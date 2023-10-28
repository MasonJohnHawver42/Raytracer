
#include "raytracer/raytracer.h"
#include "raytracer/bvh.h"

#include "resources/scene.h"

#include "resources/image.h"

#include "math/common.h"
#include "math/noise.h"
#include "math/vector.h"

#include <math.h>
#include <float.h>

#include <stdlib.h>


void rtctx_load(scene* raw_s, rtctx* s) 
{
    s->width = raw_s->width;
    s->height = raw_s->height;
    s->bg_color = raw_s->bg_color;
    s->m_dc = raw_s->m_depth_cue;
    s->m_camera.v_fov = raw_s->v_fov;

    //build the bvh
    int depth = floor(log2f(fmaxf(((float)(raw_s->m_sphere_objs.size + raw_s->m_faces.size)), 1.0f) + 1.0f));
    depth = fmax(1, fmin(depth, 32));

    bvh_build(depth, &raw_s->m_sphere_objs, &raw_s->m_faces, &raw_s->m_vertexes, &raw_s->m_normals, &s->m_bvh);
    // arena_free(&raw_s->m_sphere_objs);

    //preprocess triangles
    // s->m_tc = raw_s->m_faces.size;
    // s->m_nc = raw_s->m_normals.size;
    // s->m_uvc = raw_s->m_uvs.size;

    s->m_normals = arena_array(&raw_s->m_normals);
    arena_free(&raw_s->m_normals);

    s->m_uvs = arena_array(&raw_s->m_uvs);
    arena_free(&raw_s->m_uvs);

    //move lights over
    arena_move(&raw_s->m_lights, &s->m_lights);

    //move texture cache over
    hashmap_move(&raw_s->m_textures, &s->m_textures);

    //set up camera
    s->m_camera.m_pos = raw_s->pos;
    s->m_camera.m_fwd = raw_s->fwd;
    vec3_cross(&raw_s->fwd, &raw_s->up, &s->m_camera.m_right);
    vec3_cross(&s->m_camera.m_right, &raw_s->fwd, &s->m_camera.m_up);
    vec3_norm(&s->m_camera.m_fwd);
    vec3_norm(&s->m_camera.m_up);
    vec3_norm(&s->m_camera.m_right);

    s->m_camera.m_height = 2 * tan((raw_s->v_fov / 2.0f) * (M_PI / 180.0f));
    s->m_camera.m_width = s->m_camera.m_height * ((float)s->width / (float)s->height);
}

void rtctx_init_ray(vec2* uv, rtctx* s, ray* r) 
{
    r->m_pos = s->m_camera.m_pos;
    r->m_vel = s->m_camera.m_fwd;

    float scale_x = (uv->x - 0.5f) * s->m_camera.m_width;
    float scale_y = (0.5f - uv->y) * s->m_camera.m_height;

    r->m_vel.x += (scale_x * s->m_camera.m_right.x) + (scale_y * s->m_camera.m_up.x);
    r->m_vel.y += (scale_x * s->m_camera.m_right.y) + (scale_y * s->m_camera.m_up.y);
    r->m_vel.z += (scale_x * s->m_camera.m_right.z) + (scale_y * s->m_camera.m_up.z);

    vec3_norm(&r->m_vel); //norm it
}

void rtctx_cast_ray(ray* s_ray, rtctx* s, unsigned int avoid, int avoid_type, int* hit, raycast* rc) 
{
    
    vec3 inv_d;
    vec3_inv(&inv_d, &s_ray->m_vel);

    //query bvh (go ahead try to read the code I dare you)
    bvh_cast_ray(s_ray, &inv_d, &s->m_bvh, avoid, avoid_type, hit, rc);

    if (*hit) 
    {
        if (rc->m_type == SPHERE_RC) 
        {
            sphere_obj* so = s->m_bvh.m_spheres + rc->m_id;
            rc->mat = so->m_mat;
            // printf("%d %d %f %f \n", rc->m_type, rc->m_id, so->m_sphere.m_rad, rc->time_hit);

            //calc hit pos and normal
            vec3_scaled_add(1.0f, &s_ray->m_pos, rc->time_hit, &s_ray->m_vel, &rc->hit_pos);
            vec3_sub(&rc->hit_pos, &so->m_sphere.m_pos, &rc->normal);
            vec3_scale(1.0f / so->m_sphere.m_rad, &rc->normal);

            //calc uv
            double what;
            rc->uv.x = modf(((atan2(rc->normal.y, rc->normal.x) + M_PI) / (2 * M_PI)) + 0.5f, &what);
            rc->uv.y = acos(rc->normal.z) / M_PI;
        }
        else if (rc->m_type == TRIANGLE_RC) 
        {
            triangle_obj* tri = s->m_bvh.m_triangles + rc->m_id;
            rc->mat = tri->m_mat;

            vec3_scaled_add(1.0f, &s_ray->m_pos, rc->time_hit, &s_ray->m_vel, &rc->hit_pos);

            if (tri->nv1 == tri->nv2 && tri->nv2 == tri->nv3) 
            {
                rc->normal = s->m_normals[tri->nv1 - 1];
            }
            else 
            {
                vec3_bc_mix(&rc->bc, 
                    &s->m_normals[tri->nv1 - 1], 
                    &s->m_normals[tri->nv2 - 1], 
                    &s->m_normals[tri->nv3 - 1], 
                    &rc->normal);
                vec3_norm(&rc->normal);
            }

            if (tri->uv1 == 0 || tri->uv2 == 0 || tri->uv3 == 0) 
            {
                rc->uv.x = 0;
                rc->uv.y = 0;
            }
            else 
            {
                double what;
                vec2_bc_mix(&rc->bc, &s->m_uvs[tri->uv1 - 1], &s->m_uvs[tri->uv2 - 1], &s->m_uvs[tri->uv3 - 1], &rc->uv);
                rc->uv.x = modf(rc->uv.x, &what);
                rc->uv.y = modf(rc->uv.y, &what);

            }
        }
    }
}

void rtctx_shade(rtctx* s, ray* s_ray, int hit, raycast* rc, vec3* color) 
{
    // printf("%d ", hit);
    if (hit) 
    {
        vec3 diffuse_color;

        if (rc->mat.m_tex_id == 0) 
        {
            diffuse_color = rc->mat.m_diffuse;
        }
        else 
        {
            Image* img = (Image*)hashmap_get_hash(rc->mat.m_tex_id, &s->m_textures);
            double what;
            
            rc->uv.x = clamp(rc->uv.x, 0, 1);
            rc->uv.y = clamp(rc->uv.y, 0, 1);
            unsigned int i = image_getindex(&rc->uv, img);
            if (!(i >= 0 && i < img->width * img->height)) 
            {
                printf("%f %f %d %d \n", rc->uv.x, rc->uv.y, i, img->width * img->height - i);
            }
            // printf("%d \n", i >= 0 && i < img->width * img->height ? 1 : 0);
            image_getpixel(i, &diffuse_color, img);
        }

        vec3 result = diffuse_color;
        vec3_scale(rc->mat.ka, &result); //add ambient part

        vec3 l, h, sub_color; // light vec, half vec, and temp color var for each light
        ray light_ray; //ray towards light

        //output from light ray cast
        int lr_hit;
        raycast light_rc;

        float light_time; //distance to the light

        //loop through lights
        arena_iter iter;
        int cont = arena_begin(&iter, &s->m_lights);
        while(cont)
        {
            light* pl = (light*)arena_get(&iter, &s->m_lights);
            light_ray.m_pos = rc->hit_pos;

            //calc light vector
            if (pl->m_type == POINT || pl->m_type == AT_POINT) 
            {
                vec3_sub(&pl->m_pos, &light_ray.m_pos, &l);
                light_time = sqrt(l.x * l.x + l.y * l.y + l.z * l.z);
                vec3_scale(1.0f / light_time, &l);
            }
            if (pl->m_type == DIRECTIONAL || pl->m_type == AT_DIRECTIONAL) 
            {
                l = pl->m_vel;
                vec3_scale(-1.0f, &l);
                vec3_norm(&l);
                light_time = FLT_MAX;
            }
            
            //calc half vector
            vec3_sub(&l, &s_ray->m_vel, &h);
            vec3_scale(0.5f, &h);
            vec3_norm(&h);

            //set light ray values
            light_ray.m_vel = l;

            //cast ray
            rtctx_cast_ray(&light_ray, s, rc->m_id, rc->m_type, &lr_hit, &light_rc);

            // printf("%f %d %d %d\n", light_rc.time_hit, light_rc.m_type, SPHERE_RC, TRIANGLE_RC);

            float dot = vec3_dot(&rc->normal, &l);
            float dot2;
            float att = 1.0f;

            //check if the ray hit
            if ((!lr_hit || light_rc.time_hit >= light_time) && dot >= 0.0f) 
            {
                
                // vec3_print(&rc->hit_pos);

                //calc light if it has it attenuation
                if (pl->m_type == AT_POINT) 
                {
                    att = 1.0f / (pl->m_att.x + pl->m_att.y * light_time + pl->m_att.z * light_time * light_time);
                }

                //calc diffuse and specular (ambient already added)
                dot2 = vec3_dot(&rc->normal, &h);
                vec3_scaled_add(rc->mat.kd * dot, &diffuse_color, dot2 > 0.0 ? rc->mat.ks * powf(dot2, rc->mat.n) : 0.0f, &rc->mat.m_specular, &sub_color);                
                vec3_scale(att, &sub_color);
                vec3_mult(&sub_color, &pl->m_color);

                vec3_add(&sub_color, &result, &result);
            }

            cont = arena_next(&iter, &s->m_lights);
        }

        //apply depth cue
        if (s->m_dc.apply) 
        {
            float adc;
            if (rc->time_hit <= s->m_dc.d_min) { adc = s->m_dc.a_max; }
            else if (rc->time_hit >= s->m_dc.d_max) { adc = s->m_dc.a_min; }
            else 
            {
                adc = s->m_dc.a_min + (s->m_dc.a_max - s->m_dc.a_min) * ((s->m_dc.d_max - rc->time_hit) / (s->m_dc.d_max - s->m_dc.d_min));
            }

            vec3_scaled_add(adc, &result, 1 - adc, &s->m_dc.m_color, &result);
        }

        *color = result;
    }
    else 
    {
        *color = s->bg_color;
    }
}

void rtctx_free(rtctx *s) 
{

    arena_iter iter;
    int cont = arena_begin(&iter, &s->m_textures.values);
    while (cont) 
    {
        Image* img = (Image*)arena_get(&iter, &s->m_textures.values);
        image_free(img);
        cont = arena_next(&iter, &s->m_textures.values);
    }

    hashmap_free(&s->m_textures);
    arena_free(&s->m_lights);
    bvh_free(&s->m_bvh);

    // free(s->m_triangles);
    free(s->m_normals);
    free(s->m_uvs);
}