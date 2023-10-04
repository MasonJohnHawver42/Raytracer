#include "math/quaternion.h"

#include "math/vector.h"
#include "math/matrix.h"

#include <math.h>


void quat_init(quat* q) 
{
    q->w = 1;
    q->x = 0;
    q->y = 0;
    q->z = 0;
}

void quat_axisangle(float angle, vec3* axis, quat* q) 
{
    q->w = cos(angle / 2.0);
    float scale = sin(angle / 2.0);
    q->x = axis->x * scale;
    q->y = axis->y * scale;
    q->z = axis->z * scale;
}

void mat4_quat(quat* q, mat4* mat, int safe)
{
    if (safe) { quat_norm(q); }
    mat->vals[0] = 1 - (2 * q->y * q->y) - (2 * q->z * q->z);
    mat->vals[1] = (2 * q->x * q->y) - (2 * q->w * q->z);
    mat->vals[2] = (2 * q->x * q->z) + (2 * q->w * q->y);
    mat->vals[3] = 0;

    mat->vals[4] = (2 * q->x * q->y) + (2 * q->w * q->z);
    mat->vals[5] = 1 - (2 * q->x * q->x) - (2 * q->z * q->z);
    mat->vals[6] = (2 * q->y * q->z) - (2 * q->w * q->x); 
    mat->vals[7] = 0;

    mat->vals[8] = (2 * q->x * q->z) - (2 * q->w * q->y);
    mat->vals[9] = (2 * q->y * q->z) + (2 * q->w * q->x);
    mat->vals[10] = 1 - (2 * q->x * q->x) - (2 * q->y * q->y);
    mat->vals[11] = 0;

    mat->vals[12] = 0;
    mat->vals[13] = 0;
    mat->vals[14] = 0;
    mat->vals[15] = 1;
}


void quat_mul(quat* a, quat* b, quat* res) {
    res->w = (-a->x * b->x) - a->y * b->y - a->z * b->z + a->w * b->w;
    res->x =  a->x * b->w + a->y * b->z - a->z * b->y + a->w * b->x;
    res->y = (-a->x * b->z) + a->y * b->w + a->z * b->x + a->w * b->y;
    res->z =  a->x * b->y - a->y * b->x + a->z * b->w + a->w * b->z;
}

void quat_conj(quat *a, quat* res) 
{
    res->w = a->w;
    res->x = -a->x;
    res->y = -a->y;
    res->z = -a->z;
}

void quat_scale(float a, quat* q) 
{
    q->w = q->w * a;
    q->x = q->x * a;
    q->y = q->y * a;
    q->z = q->z * a;
}


void quat_norm(quat* q) 
{
    float n = sqrt(q->w * q->w + q->x * q->x + q->y * q->y + q->z * q->z);
    quat_scale(1.0f / n, q);
}

void quat_print(quat* v) 
{
    printf("[%.2f; %.2f; %.2f; %.2f]\n", v->w, v->x, v->y, v->w);
}

