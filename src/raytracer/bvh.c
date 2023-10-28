#include "raytracer/bvh.h"

#include "core/array.h"
#include "math/geometry.h"
#include "resources/scene.h"

#include <stdlib.h>
#include <float.h>

#define BINS 16


void init_bounds(aabb* ab) 
{
    ab->min_pt.x = FLT_MAX;
    ab->min_pt.y = FLT_MAX;
    ab->min_pt.z = FLT_MAX;

    ab->max_pt.x = -FLT_MAX;
    ab->max_pt.y = -FLT_MAX;
    ab->max_pt.z = -FLT_MAX;
}

void grow_bounds(aabb *src, aabb* ab) 
{
    src->min_pt.x = fminf(src->min_pt.x, ab->min_pt.x);
    src->min_pt.y = fminf(src->min_pt.y, ab->min_pt.y);
    src->min_pt.z = fminf(src->min_pt.z, ab->min_pt.z);

    src->max_pt.x = fmaxf(src->max_pt.x, ab->max_pt.x);
    src->max_pt.y = fmaxf(src->max_pt.y, ab->max_pt.y);
    src->max_pt.z = fmaxf(src->max_pt.z, ab->max_pt.z);
}

void update_bounds(unsigned int index, bvh* b) 
{
    bvh_node* node = b->m_nodes + index;

    init_bounds(&node->bounds);


    aabb ab;
    for (unsigned int i = node->first_index; i < node->last_index; i++) 
    {
        aabb_sphere(&b->m_spheres[i].m_sphere, &ab);
        grow_bounds(&node->bounds, &ab);
    }
    for (unsigned int i = node->tri_first; i < node->tri_last; i++) 
    {
        grow_bounds(&node->bounds, &b->m_tri_ex[i].ab);
    }
}

struct sah_bin 
{
    aabb bounds;
    unsigned int count;
};

typedef struct sah_bin sah_bin;

// void find_split(bvh_node* node, bvh* b, int* axis_ptr, float* split_pos_ptr) 
// {
//     int   axis = -1;
//     float pos = 0;
//     float best_cost = FLT_MAX;

//     sah_bin bins[BINS];
//     aabb ab;
//     int bin_index;
//     float scale;

//     float left_area[BINS - 1];
//     float right_area[BINS - 1];
//     int   left_count[BINS - 1];
//     int   right_count[BINS - 1];
    
//     aabb left, right;
//     int left_sum, right_sum;

//     float plane_cost;

//     for (int a = 0; a < 3; a++) 
//     {
//         scale = (float)BINS / (node->bounds.max_pt.raw[a] - node->bounds.min_pt.raw[a]);

//         //bins init
//         for (int b = 0; b < BINS; b++) 
//         {
//             init_bounds(&bins[b].bounds);
//             bins[b].count = 0;
//         }

//         //bins made
//         for (int i = node->first_index; i < node->last_index; i++) 
//         {
//             aabb_sphere(&b->m_spheres[i].m_sphere, &ab);
//             bin_index = (int)((b->m_spheres[i].m_sphere.m_pos.raw[a] - node->bounds.min_pt.raw[a]) * scale);
//             bin_index = bin_index > BINS - 1 ? BINS - 1 : bin_index;
//             bins[bin_index].count++;
//             grow_bounds(&bins[bin_index].bounds, &ab);
//         }

//         for (int i = node->tri_first; i < node->tri_last; i++) 
//         {
//             bin_index = (int)((b->m_tri_ex[i].centroid.raw[a] - node->bounds.min_pt.raw[a]) * scale);
//             bin_index = bin_index > BINS - 1 ? BINS - 1 : bin_index;
//             bins[bin_index].count++;
//             grow_bounds(&bins[bin_index].bounds, &b->m_tri_ex[i].ab);
//         }

//         //calc prefixes
//         init_bounds(&left);
//         init_bounds(&right);
//         left_sum = 0;
//         right_sum = 0;

//         for (int b = 0; b < BINS - 1; b++) 
//         {
//             left_sum += bins[b].count;
//             left_count[b] = left_sum;
//             grow_bounds(&left, &bins[b].bounds);
//             left_area[b] = aabb_sa(&left);

//             right_sum += bins[BINS - 1 - b].count;
//             right_count[BINS - 2 - b] = right_sum;
//             grow_bounds(&right, &bins[BINS - 1 - b].bounds);
//             left_area[BINS - 2 - b] = aabb_sa(&right);
//         }

//         //calc sah costs
//         scale = 1.0f / scale;

//         for (int b = 0; b < BINS - 1; b++) 
//         {
//             // plane_cost = (left_count[b] * left_area[b]) + (right_count[b] * right_area[b]) + (left_area[b] * right_area[b] * fabs(left_count[b] - right_count[b]));
//             plane_cost = left_area[b] + right_area[b];
//             if (plane_cost < best_cost) 
//             {
//                 axis = a;
//                 pos = node->bounds.min_pt.raw[a] + scale * (b + 1);
//                 best_cost = plane_cost;
//             }
//         }
//     }

//     *axis_ptr = axis;
//     *split_pos_ptr = pos;
// }

void split_node(unsigned int index, bvh* b, unsigned int depth) 
{
    bvh_node* node = b->m_nodes + index;
    
    //find split axis and position
    int axis;
    float split_pos;

    // find_split(node, b, &axis, &split_pos);

    vec3 extent;
    vec3_sub(&node->bounds.max_pt, &node->bounds.min_pt, &extent);

    axis = 1;
    if (extent.y > extent.x) { axis = 1; }
    if (extent.z > extent.raw[axis]) { axis = 2; }

    split_pos = node->bounds.min_pt.raw[axis] + (extent.raw[axis] * 0.5f);

    // printf("AXIS %d %f for %d\n", axis, split_pos, index);
    // vec3_print(&node->bounds.min_pt);
    // vec3_print(&node->bounds.max_pt);
    // vec3_print(&extent);

    //partition
    int i = node->first_index;
    int j = node->last_index - 1;

    while (i <= j) 
    {
        // vec3_print(&b->m_spheres[i].m_sphere.m_pos);
        if (b->m_spheres[i].m_sphere.m_pos.raw[axis] < split_pos) { i++; }
        else 
        {
            // printf("%d %d\n", i, j);
            if (i < j) 
            {
                sphere_obj temp = b->m_spheres[i];
                b->m_spheres[i] = b->m_spheres[j];
                b->m_spheres[j] = temp;
            }
            j--;
        }
    }

    //partition triangles
    int i_tri = node->tri_first;
    int j_tri = node->tri_last - 1;

    while (i_tri <= j_tri) 
    {
        // vec3_print(&b->m_tri_ex[i_tri].centroid.raw[axis]);
        if (b->m_tri_ex[i_tri].centroid.raw[axis] < split_pos) { i_tri++; }
        else 
        {
            // printf("%d %d\n", i, j);
            if (i_tri < j_tri) 
            {
                triangle_obj temp = b->m_triangles[i_tri];
                triangle_ex  temp2 = b->m_tri_ex[i_tri];
                
                b->m_triangles[i_tri] = b->m_triangles[j_tri];
                b->m_triangles[j_tri] = temp;

                b->m_tri_ex[i_tri] = b->m_tri_ex[j_tri];
                b->m_tri_ex[j_tri] = temp2;
            }
            j_tri--;
        }
    }

    //init children
    node->left_node  = b->node_index++;
    bvh_node* left_node = b->m_nodes + node->left_node;
    left_node->left_node = 0;
    left_node->right_node = 0;
    left_node->first_index = node->first_index;
    left_node->last_index = i;
    left_node->tri_first = node->tri_first;
    left_node->tri_last = i_tri;
    
    
    node->right_node = b->node_index++;
    bvh_node* right_node = b->m_nodes + node->right_node;
    right_node->left_node = 0;
    right_node->right_node = 0;
    right_node->first_index = i;
    right_node->last_index = node->last_index;
    right_node->tri_first = i_tri;
    right_node->tri_last = node->tri_last;

    update_bounds(node->left_node, b);
    update_bounds(node->right_node, b);

    //recurse
    if ((left_node->first_index != left_node->last_index || left_node->tri_first != left_node->tri_last) && depth > 0) 
    {
        split_node(node->left_node, b, depth - 1);
    }
    if ((right_node->first_index != right_node->last_index || right_node->tri_first != right_node->tri_last) && depth > 0) 
    {
        split_node(node->right_node, b, depth - 1);
    }
}

void bvh_cast_ray(ray* s_ray, vec3* inv_d, bvh* b, unsigned int avoid, int avoid_type, int* hit, raycast* rc) 
{
    bvh_node* node = b->m_nodes;
    unsigned int stack_index = 0;

    bvh_node* stack[32];

    bvh_node *child1, *child2;

    float t0_1, t1_1, t0_2, t1_2;

    int hit_1, hit_2;

    float min_time = FLT_MAX;
    unsigned int hit_index = -1;
    int hit_type = NONE_RC;
    int count;
    
    int tri_hit;
    vec2 tri_bc, rc_bc;
    float tri_ht;

    // unsigned int prim_count = 0;

    aabb_ray_intersect(s_ray, inv_d, &node->bounds, &t0_1, &t1_1);

    *hit = 0;
    if (t0_1 > t1_1 || (t0_1 < 0.0f && t1_1 < 0.0)) 
    {
        return;
    }

    while (1)
    {
        if (node->left_node == 0 && node->right_node == 0) //is leaf
        {
            for (unsigned int i = node->first_index; i < node->last_index; i++) 
            {
                if  (avoid_type == SPHERE_RC && i == avoid) { continue; }
                // if (i == avoid) { continue; }
                sphere_ray_intersect(s_ray, &b->m_spheres[i].m_sphere, &count, &t0_1, &t1_1);
                // prim_count++;
                if (count > 0) 
                {
                    if (t0_1 < 0.0f) { t0_1 = t1_1; }
                    if (t0_1 >= 0.0f && min_time > t0_1) 
                    {
                        min_time = t0_1;
                        hit_index = i;
                        hit_type = SPHERE_RC;
                        *hit = 1;
                    }
                }

            }

            for (unsigned int i = node->tri_first; i < node->tri_last; i++) 
            {
                if (avoid_type == TRIANGLE_RC && i == avoid) { continue; }
                tri_hit = triangle_ray_intersect(s_ray, &b->m_triangles[i].v1, &b->m_triangles[i].v2, &b->m_triangles[i].v3, &tri_ht, &tri_bc);
                if (tri_hit && tri_ht < min_time) 
                {
                    min_time = tri_ht;
                    hit_index = i;
                    hit_type = TRIANGLE_RC;
                    rc_bc = tri_bc;
                    *hit = 1;
                }

            }

            if (stack_index == 0) { break; }
            else 
            {
                node = stack[--stack_index];
            }

            continue;
        }

        child1 = b->m_nodes + node->left_node;
        child2 = b->m_nodes + node->right_node;

        aabb_ray_intersect(s_ray, inv_d, &child1->bounds, &t0_1, &t1_1);
        aabb_ray_intersect(s_ray, inv_d, &child2->bounds, &t0_2, &t1_2);

        hit_1 = (t0_1 <= t1_1 && (t0_1 >= 0.0f || t1_1 >= 0.0f)) ? 1 : 0;
        hit_2 = (t0_2 <= t1_2 && (t0_2 >= 0.0f || t1_2 >= 0.0f)) ? 1 : 0;

        if (t0_1 < 0.0f) { t0_1 = t1_1; }
        if (t0_2 < 0.0f) { t0_2 = t1_2; }

        hit_1 = (hit_1 && min_time > t0_1) ? 1 : 0;
        hit_2 = (hit_2 && min_time > t0_2) ? 1 : 0;

        

        if (hit_1 && hit_2) 
        {
            if (t0_1 < t0_2) 
            {
                node = child1;
                stack[stack_index++] = child2;
                continue;
            }
            else 
            {
                node = child2;
                stack[stack_index++] = child1;
                continue;
            }
        }
        if (hit_1) { node = child1; continue; }
        if (hit_2) { node = child2; continue; }

        if(stack_index == 0) { break; }
        node = stack[--stack_index];
    }

    rc->time_hit = min_time;

    // *time_hit = min_time;
    if (*hit == 1) 
    {
        rc->m_type = hit_type;
        rc->m_id = hit_index;
        rc->bc = rc_bc;
    }

    // printf("%d \n", prim_count);

    return;

}

void bvh_build(unsigned int depth, arena* spheres, arena* faces, arena* vertexes, arena* normals, bvh* b)
{
    b->depth = depth;
    b->sphere_count = spheres->size;
    b->triangle_count = faces->size;

    b->node_count = (2 << depth) - 1;;
    b->node_index = 0;

    // printf("nodes: %d  balls: %d\n", b->node_count, spheres->size);

    b->m_nodes = malloc(sizeof(bvh_node) * b->node_count);
    // b->m_spheres = malloc(sizeof(sphere_obj) * spheres->size);
    
    b->m_nodes[0].left_node = 0;
    b->m_nodes[0].right_node = 0;
    b->m_nodes[0].first_index = 0;
    b->m_nodes[0].last_index = spheres->size;
    b->m_nodes[0].tri_first = 0;
    b->m_nodes[0].tri_last = faces->size;


    if (faces->size > 0) 
    {
        vec3* verts = arena_array(vertexes);
        arena_free(vertexes);

        b->m_triangles = malloc(sizeof(triangle_obj) * faces->size);
        b->m_tri_ex = malloc(sizeof(triangle_ex)* faces->size);

        int i = 0;
        arena_iter iter;
        int cont = arena_begin(&iter, faces);

        vec3 center;

        while (cont) 
        {
            face* s_face = arena_get(&iter, faces);

            b->m_triangles[i].m_mat = s_face->mat;

            b->m_triangles[i].v1 = verts[s_face->v1 - 1];
            b->m_triangles[i].v2 = verts[s_face->v2 - 1];
            b->m_triangles[i].v3 = verts[s_face->v3 - 1];

            if (s_face->nv1 == 0 || s_face->nv2 == 0 || s_face->nv3 == 0) 
            {
                vec3 edge1, edge2, normal;
                vec3_sub(&b->m_triangles[i].v2, &b->m_triangles[i].v1, &edge1);
                vec3_sub(&b->m_triangles[i].v3, &b->m_triangles[i].v1, &edge2);
                vec3_cross(&edge1, &edge2, &normal);
                vec3_norm(&normal);

                unsigned int index = normals->size + 1;

                b->m_triangles[i].nv1 = index;
                b->m_triangles[i].nv2 = index;
                b->m_triangles[i].nv3 = index;

                vec3* dest = (vec3*)arena_push(normals);
                *dest = normal;
            }
            else 
            {
                b->m_triangles[i].nv1 = s_face->nv1;
                b->m_triangles[i].nv2 = s_face->nv2;
                b->m_triangles[i].nv3 = s_face->nv3;
            }

            b->m_triangles[i].uv1 = s_face->uv1;
            b->m_triangles[i].uv2 = s_face->uv2;
            b->m_triangles[i].uv3 = s_face->uv3;

            aabb_triangle(&b->m_triangles[i].v1, &b->m_triangles[i].v2, &b->m_triangles[i].v3, &b->m_tri_ex[i].ab);
            centroid_triangle(&b->m_triangles[i].v1, &b->m_triangles[i].v2, &b->m_triangles[i].v3, &b->m_tri_ex[i].centroid);

            cont = arena_next(&iter, faces);
            i++;
        }

        vec3_scale(1.0f / (float)i, &center);
        // vec3_print(&center);

        free(verts);
        arena_free(faces);
    }
    else 
    {
        arena_free(vertexes);
        arena_free(faces);
        b->m_triangles = NULL;
        b->m_tri_ex = NULL;
    }



    b->node_index++;

    b->m_spheres = arena_array(spheres);
    arena_free(spheres);    

    update_bounds(0, b);
    // vec3_print(&b->m_nodes[0].bounds.min_pt);
    // vec3_print(&b->m_nodes[0].bounds.max_pt);
    split_node(0, b, depth - 1);

    if (b->m_tri_ex != NULL) { free(b->m_tri_ex); }
}

void bvh_free(bvh* b) 
{
    free(b->m_nodes);
    free(b->m_spheres);
    if (b->m_triangles != NULL) { free(b->m_triangles); }

    b->sphere_count = 0;
    b->node_count = 0;
    b->node_index = 0;
}