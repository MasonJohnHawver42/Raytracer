#ifndef SCENE_H
#define SCENE_H

#include "core/array.h"
#include "math/vector.h"
#include "math/geometry.h"

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

//scene data
struct scene 
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

typedef struct scene scene;


//load the raw scene {s} from the file {fn}
int  scene_load(char* fn, scene* s, char* msg);
//free raw scene {s}
void scene_free(scene *s);

#endif