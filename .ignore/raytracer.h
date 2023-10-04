#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "math/vector.hs"
#include "resources/scene.h"
#include "core/array.h"

struct raytracer_ctx 
{
    vec3 pos, fwd, up, right;
    float scale_x, scale_y;

    vec3 bg_color;
    arena m_sphere_objs;
};


void raytracer_ctx_init(scene* s, raytracer_ctx* ctx);

void rtctx_ray(vec2* uv, ray* r, raytracer_ctx* ctx);

void rtctx_castray(ray* r, raytracer_ctx* ctx, int* hit, float* time_hit, sphere_obj* obj_hit);

#endif