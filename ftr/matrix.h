#ifndef MATRIX_H 
#define MATRIX_H

#include <stdio.h>

#include "math/vector.h"

struct mat4 
{
    float vals[16];
};

typedef struct mat4 mat4;

void mat4_fill(float a, mat4* mat);
void mat4_identity(mat4* mat);
void mat4_translate(float x, float y, float z, mat4* mat);

void mat4_print(mat4* mat);

void mat4_matmul(mat4* a, mat4* b, mat4* res);

void vec4_matmul(vec4* a, mat4* mat, vec4* res);

void vec3_matmul(vec3* a, mat4* mat, vec4* res);


#endif