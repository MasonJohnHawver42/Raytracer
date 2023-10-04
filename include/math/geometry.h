#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "math/vector.h"

#include <math.h>

struct ray 
{
    vec3 m_pos;
    vec3 m_vel;
};

struct sphere 
{
    vec3 m_pos;
    float m_rad;
};

struct aabb 
{
    vec3 min_pt;
    vec3 max_pt;
};

typedef struct ray ray;
typedef struct sphere sphere;
typedef struct aabb aabb;

void ray_init(vec3* p, vec3* v, ray* ray);

void sphere_init(vec3* p, float r, sphere* s);

void aabb_init(vec3* min_pt, vec3* max_pt, aabb* ab);

void aabb_sphere(sphere* s, aabb* ab);

//assumes ray dir is normalized
//returns # of intersections (0, 1, 2) {count} and the first time it was hit {enter} and the second time it was hit {exit} if it was
void sphere_ray_intersect(ray* r, sphere* s, int* count, float* enter, float* exit);


#endif

