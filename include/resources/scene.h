#ifndef SCENE_H
#define SCENE_H

#include "core/array.h"
#include "math/vector.h"
#include "math/geometry.h"

//camera basis
struct camera 
{
    vec3 m_pos;
    vec3 m_fwd;
    vec3 m_up;
    vec3 m_right;
};

typedef struct camera camera;

//material of object
struct material 
{
    vec3 m_color;
    //later it can hold texture ids and etc
};

typedef struct material material;

//sphere object
struct sphere_obj 
{
    sphere m_sphere;
    material m_mat;
};

typedef struct sphere_obj sphere_obj;

//scene data 1 to 1 mapping from input file
struct raw_scene 
{
    //misc settings
    unsigned int width, height;
    vec3 bg_color;

    //camera settings
    vec3 pos, fwd, up;
    float v_fov;

    //objs
    arena  m_sphere_objs;
};

//scene data that stores precomputed and optimized structures created from raw_scene
struct scene
{

    unsigned int width, height;
    vec3 bg_color;

    camera m_camera;
    float v_fov, scale_x, scale_y;

    arena m_sphere_objs;
};

typedef struct raw_scene raw_scene;
typedef struct scene scene;

//load the raw scene {s} from the file {fn}
int  raw_scene_load(char* fn, raw_scene* s, char* msg);
//free raw scene {s}
void raw_scene_free(raw_scene *s);

//load scene {s} from raw_scene {raw_s}
void scene_load(raw_scene* raw_s, scene* s);

//make ray {r} from a pixel's {uv} and scene {s}
void scene_init_ray(vec2* uv, scene* s, ray* r);

//cast ray {r} into the scene {s} get if it was {hit} (0, 1) the time it was hit {time_hit} the {normal} vector of the intersection and the material {mat} of the object hit
void scene_cast_ray(ray* r, scene* s, int* hit, float* time_hit, vec3* normal, material* mat);

//compute {color} based on the output of scene_cast_ray
void scene_shade(scene* s, int hit, float time_hit, vec3* normal, material* mat, vec3* color);

//free scene {s} data
void scene_free(scene *s);

#endif