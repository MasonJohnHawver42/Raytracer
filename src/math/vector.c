#include "math/vector.h"

#include <stdio.h>
#include <math.h>

void vec2_init(float x, float y, vec2* v) 
{
    v->x = x;
    v->y = y;
}

void vec3_init(float x, float y, float z, vec3* v) { v->x = x; v->y = y; v->z = z; }
// void vec3_cpy(vec3* other, vec3* v) { v->x = other->x; v->y = other->y; v->z = other->z;}
void vec3_fill(float a, vec3* v) { v->x = a; v->y = a; v->z = a; }
void vec3_lerp(float t, vec3* a, vec3* b, vec3* res) 
{   
    res->x = a->x + t * (b->x - a->x);
    res->y = a->y + t * (b->y - a->y);
    res->z = a->z + t * (b->z - a->z);
}

void  vec3_add(vec3* a, vec3* b, vec3* res) 
{
    res->x = a->x + b->x;
    res->y = a->y + b->y;
    res->z = a->z + b->z;
}

void  vec3_sub(vec3* a, vec3* b, vec3* res) 
{
    res->x = a->x - b->x;
    res->y = a->y - b->y;
    res->z = a->z - b->z;
}

void vec3_scaled_add(float as, vec3* a, float bs, vec3* b, vec3* res) 
{
    res->x = (as * a->x) + (bs * b->x);
    res->y = (as * a->y) + (bs * b->y);
    res->z = (as * a->z) + (bs * b->z);
}

float vec3_dot(vec3* a, vec3* b) 
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void vec3_cross(vec3* a, vec3* b, vec3* res) 
{
    res->x = (a->y * b->z) - (a->z * b->y);
    res->y = -1 * ((a->x * b->z) - (a->z * b->x));
    res->z = (a->x * b->y) - (a->y * b->x);
}

void vec3_norm(vec3* v) 
{
    float s = 1.0f / sqrt(vec3_dot(v, v));
    vec3_scale(s, v);
}

void vec3_scale(float s, vec3* v) 
{
    v->x *= s;
    v->y *= s;
    v->z *= s;
}

void vec3_print(vec3 *v) 
{
    printf("[%.2f; %.2f; %.2f]\n", v->x, v->y, v->z);
}


void vec3_inv(vec3* a, vec3* b) 
{
    a->x = 1.0f / b->x;
    a->y = 1.0f / b->y;
    a->z = 1.0f / b->z;
}

void  vec3_mult(vec3* a, vec3* b) 
{
    a->x *= b->x;
    a->y *= b->y;
    a->z *= b->z;
}

void vec4_init(float x, float y, float z, float w, vec4* v) 
{
    v->x = x;
    v->y = y;
    v->z = z;
    v->w = w;
}

void vec4_print(vec4 *v) 
{
    printf("[%.2f; %.2f; %.2f; %.2f]\n", v->x, v->y, v->z, v->w);
}

void vec3_bc_mix(vec2* bc, vec3* v0, vec3* v1, vec3* v2, vec3* res) 
{
    float a = (1.0f - bc->x - bc->y);
    res->x = (a * v0->x) + (bc->x * v1->x) + (bc->y * v2->x);
    res->y = (a * v0->y) + (bc->x * v1->y) + (bc->y * v2->y);
    res->z = (a * v0->z) + (bc->x * v1->z) + (bc->y * v2->z);
}

void vec2_bc_mix(vec2* bc, vec2* v0, vec2* v1, vec2* v2, vec2* res) 
{
    float a = (1.0f - bc->x - bc->y);
    res->x = (a * v0->x) + (bc->x * v1->x) + (bc->y * v2->x);
    res->y = (a * v0->y) + (bc->x * v1->y) + (bc->y * v2->y);
}