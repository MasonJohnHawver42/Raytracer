#ifndef QUATERNION_H
#define QUATERNION_H

#include "math/vector.h"
#include "math/matrix.h"

#include <math.h>

typedef vec4 quat;

void quat_init(quat* q);

void quat_axisangle(float angle, vec3* axis, quat* q);

void quat_mul(quat* a, quat* b, quat* res);

void quat_conj(quat *a, quat* res);

void quat_scale(float a, quat* q);

void quat_norm(quat* q);

void quat_print(quat* p);


void mat4_quat(quat* q, mat4* mat, int safe);

#endif