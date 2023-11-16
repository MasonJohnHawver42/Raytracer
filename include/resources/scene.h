#ifndef SCENE_H
#define SCENE_H

#include "core/array.h"
#include "core/hashmap.h"
#include "math/vector.h"
#include "math/geometry.h"


//material of object
struct material 
{
    vec3 m_diffuse;
    vec3 m_specular;
    float ka, kd, ks, n, alpha, f0, ior;
    unsigned int m_tex_id;
    unsigned int m_normal_tex_id;
};

typedef struct material material;

struct face 
{
    unsigned int v1, v2, v3;
    unsigned int nv1, nv2, nv3;
    unsigned int uv1, uv2, uv3;

    material mat;
};

typedef struct face face;

//sphere object
struct sphere_obj 
{
    sphere m_sphere;
    material m_mat;
};

struct triangle_obj //naive but fuck it
{
    vec3 v1, v2, v3;
    
    unsigned int nv1, nv2, nv3;
    unsigned int uv1, uv2, uv3;

    // vec3 center;
    
    material m_mat;
};

enum light_type_ {POINT = 0, DIRECTIONAL = 1, AT_POINT = 2, AT_DIRECTIONAL = 3};

struct light 
{
    union 
    {
        vec3 m_pos;
        vec3 m_vel;
    };
    vec3 m_color;
    vec3 m_att;

    int m_type;
};

struct depth_cue 
{
    vec3 m_color;
    float a_max, a_min;
    float d_max, d_min;

    int apply;
};

typedef struct sphere_obj sphere_obj;
typedef struct triangle_obj triangle_obj;
typedef struct light light;

typedef struct depth_cue depth_cue;

//scene data
struct scene 
{
    //misc settings
    unsigned int width, height;
    vec3 bg_color;
    float ior;

    depth_cue m_depth_cue;

    //camera settings
    vec3 pos, fwd, up;
    float v_fov;

    //resources
    hashmap m_textures;

    //objs
    arena  m_sphere_objs;
    arena  m_lights;

    //temp
    arena m_texture_queue;
    
    arena m_vertexes;
    arena m_normals;
    arena m_uvs;

    int m_depth;

    arena m_faces;
};

typedef struct scene scene;


//load the raw scene {s} from the file {fn}
int  scene_load(char* fn, scene* s, char* msg);
//free raw scene {s}
void scene_free(scene *s);

#endif