#include "raytracer.h"

#include "math/vector.hs"
#include "resources/scene.h"

#include "geometry.h"

#include "core/array.h"

#include <math.h>

void rtctx_init(scene* s, raytracer_ctx* ctx) 
{
    // vec3_cpy(&s->m_camera.m_pos, &ctx->pos);
    // vec3_cpy(&s->m_camera.m_dir, &ctx->fwd);
    ctx->pos = s->m_camera.m_pos;
    ctx->fwd = s->m_camera.m_dir;
    vec3_cross(s->m_camera.m_dir, &s->m_camera.m_up, &ctx->right);
    vec3_cross(&right, &s->m_camera.m_dir, &ctx->up);
    vec3_norm(&ctx->fwd);
    vec3_norm(&ctx->right);
    vec3_norm(&ctx->up);

    vec3_print(&s->m_camera.m_pos);
    vec3_print(&ctx->fwd);
    vec3_print(&ctx->right);
    vec3_print(&ctx->up);
    
    ctx->scale_x = tan(s->m_camera.m_vfov * (M_PI / 360.0f));
    ctx->scale_y = scale_x * ((float)s->height / (float)s->width);

    // vec3_cpy(&s->background_color.m_pos, &ctx->bg_color);
    ctx->bg_color = s->background_color.m_pos;

    ctx->m_sphere_objs = s->m_sphere_objs;
}

void rtctx_ray(vec2* uv, ray* r, raytracer_ctx* ctx) 
{
    // vec3_cpy(&ctx->pos, &r->m_pos);
    r->m_pos = ctx->pos;

    vec3_scaled_add(((uv->x * 2.0f) - 1) * ctx->scale_x, &ctx->right, 1.0f, &ctx->fwd, &r->m_vel); 
    vec3_scaled_add((1 - (2 * uv->y)) * ctx->scale_y, &ctx->up, 1.0f, &r->m_vel, &r->m_vel);
    vec3_norm(&r->m_vel); //norm it
}

void rtctx_castray(ray* r, raytracer_ctx* ctx, int* hit, float* time_hit, sphere_obj* obj_hit) 
{
    arena_iter iter;

    int count;
    float t0, t1;

    *hit = 0;
    sphere_obj* so_hit;

    int cont = arena_begin(&iter, &ctx->m_sphere_objs);
    while(cont) 
    {
        sphere_obj* s_sphere_obj = (sphere_obj*)arena_get(&iter, &ctx->m_sphere_objs);
            
        sphere_ray_intersect(&s_ray, &s_sphere_obj->m_sphere, &count, &t0, &t1);
            
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

        cont = arena_next(&iter, &ctx->m_sphere_objs);
    }

    if (*hit) { *obj_hit = *so_hit; }
}