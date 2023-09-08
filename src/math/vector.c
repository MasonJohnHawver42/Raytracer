#include "math/vector.h"

void vec3_lerp(float t, vec3* a, vec3* b, vec3* res) 
{   
    res->x = a->x + t * (b->x - a->x);
    res->y = a->y + t * (b->y - a->y);
    res->z = a->z + t * (b->z - a->z);
}

void vec3_setall(float a, vec3* v) { v->x = a; v->y = a; v->z = a; }
void vec3_set(float x, float y, float z, vec3* v) { v->x = x; v->y = y; v->z = z; }

