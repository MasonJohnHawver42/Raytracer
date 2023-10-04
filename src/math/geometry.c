#include "math/geometry.h"
#include "math/vector.h"

#include <math.h>

void ray_init(vec3* p, vec3* v, ray* ray) 
{
    // vec3_cpy(p, &ray->m_pos);
    // vec3_cpy(v, &ray->m_vel);
    ray->m_pos = *p;
    ray->m_vel = *v;
}

void sphere_init(vec3* p, float r, sphere* s) 
{
    // vec3_cpy(p, &s->m_pos);
    s->m_pos = *p;
    s->m_rad = r;
}

void aabb_init(vec3* min_pt, vec3* max_pt, aabb* ab) 
{
    ab->min_pt = *min_pt;
    ab->max_pt = *max_pt;
}

void aabb_sphere(sphere* s, aabb* ab) 
{
    ab->min_pt = s->m_pos;
    ab->max_pt = s->m_pos;

    ab->min_pt.x -= s->m_rad;
    ab->min_pt.y -= s->m_rad;
    ab->min_pt.z -= s->m_rad;

    ab->max_pt.x += s->m_rad;
    ab->max_pt.y += s->m_rad;
    ab->max_pt.z += s->m_rad;
}

//assumes ray dir is normalized
void sphere_ray_intersect(ray* r, sphere* s, int* count, float* enter, float* exit) 
{

    vec3 l;
    vec3_init(0, 0, 0, &l);
    vec3_sub(&r->m_pos, &s->m_pos, &l);

    float a = 1;
    float b = 2 * vec3_dot(&r->m_vel, &l);
    float c = vec3_dot(&l, &l) - (s->m_rad * s->m_rad);

    float d = (b * b) - (4 * a * c);
    if (d < 0.0f) { *count = 0; }
    else if (d == 0) { *count = 1; *enter = -1 * b / (2 * a); }
    else 
    {
        float q = (b > 0) ?
            -0.5f * (b + sqrt(d)) :
            -0.5f * (b - sqrt(d));
        *count = 2;
        *enter = q / a;
        *exit = c / q;
        
        if (*enter > *exit) 
        {
            float tmp = *enter;
            *enter = *exit;
            *exit = tmp;
        }
    }

}