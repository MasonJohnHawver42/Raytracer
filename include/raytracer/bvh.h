#ifndef BVH_H
#define BVH_H

#include "math/geometry.h"
#include "resources/scene.h"

struct bvh_node 
{
    unsigned int left_node, right_node, first_index, last_index;
    unsigned int tri_first, tri_last;
    aabb bounds;
};

typedef struct bvh_node bvh_node;

struct triangle_ex 
{
    vec3 centroid;
    aabb ab;
};

typedef struct triangle_ex triangle_ex;

struct bvh 
{
    bvh_node* m_nodes;
    sphere_obj* m_spheres;
    triangle_obj* m_triangles;
    triangle_ex* m_tri_ex;

    unsigned int sphere_count, triangle_count, node_count, node_index, depth;
};

enum object_type_ {SPHERE_RC, TRIANGLE_RC, NONE_RC};

typedef union object_data object_data;

struct raycast 
{
    float time_hit;
    unsigned int m_id;
    int m_type;

    vec2 bc;

    material mat;
    vec3 hit_pos;
    vec3 normal;
    vec2 uv;
};

typedef struct raycast raycast;

typedef struct bvh bvh;

void bvh_build(unsigned int depth, arena* spheres, arena* faces, arena* vertexes, arena* normals, bvh* b);

void bvh_cast_ray(ray* s_ray, vec3* inv_d, bvh* b, unsigned int avoid, int avoid_type, int* hit, raycast* rc);

void bvh_free(bvh* b);


#endif