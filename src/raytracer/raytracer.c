
#include "raytracer/raytracer.h"
#include "raytracer/bvh.h"

#include "resources/scene.h"


void rtctx_load(scene* raw_s, rtctx* s) 
{
    s->width = raw_s->width;
    s->height = raw_s->height;
    s->bg_color = raw_s->bg_color;
    s->m_camera.v_fov = raw_s->v_fov;
    s->m_sphere_objs = raw_s->m_sphere_objs;
    raw_s->m_sphere_objs.head = NULL;
    raw_s->m_sphere_objs.tail = NULL;

    s->m_camera.m_pos = raw_s->pos;
    s->m_camera.m_fwd = raw_s->fwd;
    vec3_cross(&raw_s->fwd, &raw_s->up, &s->m_camera.m_right);
    vec3_cross(&s->m_camera.m_right, &raw_s->fwd, &s->m_camera.m_up);
    vec3_norm(&s->m_camera.m_fwd);
    vec3_norm(&s->m_camera.m_up);
    vec3_norm(&s->m_camera.m_right);

    s->m_camera.m_height = 2 * tan((raw_s->v_fov / 2.0f) * (M_PI / 180.0f));
    s->m_camera.m_width = s->m_camera.m_height * ((float)s->width / (float)s->height);


        // vec3_print(&s->m_camera.m_fwd);
        // vec3_print(&s->m_camera.m_up);
        // vec3_print(&s->m_camera.m_right);

    // s->scale_x = tan(raw_s->v_fov * (M_PI / 360.0f));
    // s->scale_y = s->scale_x * ((float)raw_s->height / (float)raw_s->width);
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

void rtctx_cast_ray(ray* s_ray, rtctx* s, int* hit, float* time_hit, vec3* normal, material* mat) 
{
    //iter through spheres
    arena_iter iter;

    int count;
    float t0, t1;

    *hit = 0;
    sphere_obj* so_hit;

    int cont = arena_begin(&iter, &s->m_sphere_objs);
    while(cont) 
    {
        sphere_obj* s_sphere_obj = (sphere_obj*)arena_get(&iter, &s->m_sphere_objs);
            
        sphere_ray_intersect(s_ray, &s_sphere_obj->m_sphere, &count, &t0, &t1);
            
        if (count > 0) 
        {
            if (t0 < 0.0f) { t0 = t1; }
            if (t0 >= 0.0f && !(*hit && !(t0 < *time_hit))) 
            {
                *hit = 1;
                *time_hit = t0;
                so_hit = s_sphere_obj;
            }
        }

        cont = arena_next(&iter, &s->m_sphere_objs);
    }

    if (*hit) 
    {
        *mat = so_hit->m_mat;

        //calc hit pos and normal
        vec3_scaled_add(1.0f, &s_ray->m_pos, *time_hit, &s_ray->m_vel, normal);
        vec3_sub(normal, &so_hit->m_sphere.m_pos, normal);
        vec3_scale(1.0f / so_hit->m_sphere.m_rad, normal);
    }
}

void rtctx_shade(rtctx* s, int hit, float time_hit, vec3* normal, material* mat, vec3* color) 
{
    if (hit) 
    {
        *color = mat->m_color;
    }
    else 
    {
        *color = s->bg_color;
    }
}

void rtctx_free(rtctx *s) 
{
    arena_free(&s->m_sphere_objs);
}