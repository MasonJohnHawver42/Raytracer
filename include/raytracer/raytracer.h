#ifndef RAYTRACER_H
#define RAYTRACER_H

#include "core/array.h"

#include "math/vector.h"
#include "math/geometry.h"
#include "resources/scene.h"

//camera basis
struct camera 
{
    vec3 m_pos;
    vec3 m_fwd;
    vec3 m_up;
    vec3 m_right;

    float v_fov;

    float m_height, m_width;
};

typedef struct camera camera;


struct rtctx
{
    unsigned int width, height;
    vec3 bg_color;

    camera m_camera;

    arena m_sphere_objs;
};

typedef struct rtctx rtctx;

//load scene {s} from raw_scene {raw_s}
void rtctx_load(scene* raw_s, rtctx* s);

//make ray {r} from a pixel's {uv} and scene {s}
void rtctx_init_ray(vec2* uv, rtctx* s, ray* r);

//cast ray {r} into the scene {s} get if it was {hit} (0, 1) the time it was hit {time_hit} the {normal} vector of the intersection and the material {mat} of the object hit
void rtctx_cast_ray(ray* r, rtctx* s, int* hit, float* time_hit, vec3* normal, material* mat);

//compute {color} based on the output of scene_cast_ray
void rtctx_shade(rtctx* s, int hit, float time_hit, vec3* normal, material* mat, vec3* color);

//free scene {s} data
void rtctx_free(rtctx *s);



#endif