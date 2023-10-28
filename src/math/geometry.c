#include "math/geometry.h"
#include "math/vector.h"

#include <math.h>
#include <float.h>
#include <stdio.h>  

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

void aabb_triangle(vec3* v0, vec3* v1, vec3* v2, aabb* ab) 
{
    ab->min_pt.x = fminf(v0->x, fminf(v1->x, v2->x));
    ab->min_pt.y = fminf(v0->y, fminf(v1->y, v2->y));
    ab->min_pt.z = fminf(v0->z, fminf(v1->z, v2->z));

    ab->max_pt.x = fmax(v0->x, fmax(v1->x, v2->x));
    ab->max_pt.y = fmax(v0->y, fmax(v1->y, v2->y));
    ab->max_pt.z = fmax(v0->z, fmax(v1->z, v2->z));
}

void centroid_triangle(vec3* v0, vec3* v1, vec3* v2, vec3* c) 
{
    c->x = (v0->x + v1->x + v2->x) / 3.0f;
    c->y = (v0->y + v1->y + v2->y) / 3.0f;
    c->z = (v0->z + v1->z + v2->z) / 3.0f;
}


void aabb_ray_intersect(ray* r, vec3* inv_d, aabb* ab, float* tmin, float* tmax) 
{
    vec3 t0s, t1s;

    vec3_sub(&ab->min_pt, &r->m_pos, &t0s);
    vec3_sub(&ab->max_pt, &r->m_pos, &t1s);

    vec3_mult(&t0s, inv_d);
    vec3_mult(&t1s, inv_d);
    
    *tmin = fmaxf(fminf(t0s.x, t1s.x), fmaxf(fminf(t0s.y, t1s.y), fminf(t0s.z, t1s.z)));
    *tmax = fminf(fmax(t0s.x, t1s.x), fminf(fmaxf(t0s.y, t1s.y), fmaxf(t0s.z, t1s.z)));
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
    if (d < 0.0f) { *count = 0; return; }
    else if (d == 0) { *count = 1; *enter = -1 * b / (2 * a); return; }
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
        return;
    }

}

int triangle_ray_intersect(ray* r, vec3* v1, vec3* v2, vec3* v3, float* t, vec2* bc) 
{
    vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    vec3_sub(v2, v1, &edge1);
    vec3_sub(v3, v1, &edge2);
    vec3_cross(&r->m_vel, &edge2, &h);
    a = vec3_dot(&edge1, &h);

    if (a > -FLT_EPSILON && a < FLT_EPSILON) { return 0; }

    f = 1.0f / a;

    vec3_sub(&r->m_pos, v1, &s);
    u = f * vec3_dot(&s, &h);

    if (u < 0.0 || u > 1.0) { return 0; }

    vec3_cross(&s, &edge1, &q);
    v = f * vec3_dot(&r->m_vel, &q);

    if (v < 0.0 || u + v > 1.0) { return 0; }

    float time = f * vec3_dot(&edge2, &q);

    if (time > FLT_EPSILON) 
    {
        *t = time;
        bc->x = u;
        bc->y = v;
        return 1;
    }

    return 0;
}

float aabb_sa(aabb* ab) 
{
    vec3 e;
    vec3_sub(&ab->max_pt, &ab->min_pt, &e);

    return 2.0f * (e.x * e.y + e.x * e.z + e.z * e.y);
}

void sphere_print(sphere* s) 
{
    printf(" [%f %f %f] %f \n", s->m_pos.x, s->m_pos.y, s->m_pos.z, s->m_rad);

}