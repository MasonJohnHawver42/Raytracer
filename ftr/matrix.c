#include "math/matrix.h"



void mat4_fill(float a, mat4* mat)
{
    for (int i = 0; i < 16; i++) { mat->vals[i] = a; }
}

void mat4_identity(mat4* mat) 
{
    for (int i = 0; i < 16; i++) { mat->vals[i] = (i % 5 == 0) ? 1 : 0; }
}

void mat4_print(mat4* mat) 
{
    for (int i = 0; i < 4; i++) 
    {
        for (int j = 0; j < 4; j++) 
        {
            printf("%.2f%s", mat->vals[j + (i * 4)], (j == 3) ? "\n" : " ");
        }
    }
}

void mat4_matmul(mat4* a, mat4* b, mat4* res) 
{
    res->vals[0]  = a->vals[0] * b->vals[0] + a->vals[1] * b->vals[4] + a->vals[2] * b->vals[8] + a->vals[3] * b->vals[12];
    res->vals[1]  = a->vals[0] * b->vals[1] + a->vals[1] * b->vals[5] + a->vals[2] * b->vals[9] + a->vals[3] * b->vals[13];
    res->vals[2]  = a->vals[0] * b->vals[2] + a->vals[1] * b->vals[6] + a->vals[2] * b->vals[10] + a->vals[3] * b->vals[14];
    res->vals[3]  = a->vals[0] * b->vals[3] + a->vals[1] * b->vals[7] + a->vals[2] * b->vals[11] + a->vals[3] * b->vals[15];

    res->vals[4]  = a->vals[4] * b->vals[0] + a->vals[5] * b->vals[4] + a->vals[6] * b->vals[8] + a->vals[7] * b->vals[12];
    res->vals[5]  = a->vals[4] * b->vals[1] + a->vals[5] * b->vals[5] + a->vals[6] * b->vals[9] + a->vals[7] * b->vals[13];
    res->vals[6]  = a->vals[4] * b->vals[2] + a->vals[5] * b->vals[6] + a->vals[6] * b->vals[10] + a->vals[7] * b->vals[14];
    res->vals[7]  = a->vals[4] * b->vals[3] + a->vals[5] * b->vals[7] + a->vals[6] * b->vals[11] + a->vals[7] * b->vals[15];

    res->vals[8]  = a->vals[8] * b->vals[0] + a->vals[9] * b->vals[4] + a->vals[10] * b->vals[8] + a->vals[11] * b->vals[12];
    res->vals[9]  = a->vals[8] * b->vals[1] + a->vals[9] * b->vals[5] + a->vals[10] * b->vals[9] + a->vals[11] * b->vals[13];
    res->vals[10] = a->vals[8] * b->vals[2] + a->vals[9] * b->vals[6] + a->vals[10] * b->vals[10] + a->vals[11] * b->vals[14];
    res->vals[11] = a->vals[8] * b->vals[3] + a->vals[9] * b->vals[7] + a->vals[10] * b->vals[11] + a->vals[11] * b->vals[15];

    res->vals[12] = a->vals[12] * b->vals[0] + a->vals[13] * b->vals[4] + a->vals[14] * b->vals[8] + a->vals[15] * b->vals[12];
    res->vals[13] = a->vals[12] * b->vals[1] + a->vals[13] * b->vals[5] + a->vals[14] * b->vals[9] + a->vals[15] * b->vals[13];
    res->vals[14] = a->vals[12] * b->vals[2] + a->vals[13] * b->vals[6] + a->vals[14] * b->vals[10] + a->vals[15] * b->vals[14];
    res->vals[15] = a->vals[12] * b->vals[3] + a->vals[13] * b->vals[7] + a->vals[14] * b->vals[11] + a->vals[15] * b->vals[15];
}

void mat4_translate(float x, float y, float z, mat4* mat) 
{
    mat->vals[0] = 1;
    mat->vals[1] = 0;
    mat->vals[2] = 0;
    mat->vals[3] = x;

    mat->vals[4] = 0;
    mat->vals[5] = 1;
    mat->vals[6] = 0;
    mat->vals[7] = y;

    mat->vals[8] = 0;
    mat->vals[9] = 0;
    mat->vals[10] = 1;
    mat->vals[11] = z;

    mat->vals[12] = 0;
    mat->vals[13] = 0;
    mat->vals[14] = 0;
    mat->vals[15] = 1;
}

void vec4_matmul(vec4* b, mat4* a, vec4* res)
{
    res->x = a->vals[0] * b->x + a->vals[1] * b->y + a->vals[2] * b->z + a->vals[3] * b->w;
    res->y = a->vals[4] * b->x + a->vals[5] * b->y + a->vals[6] * b->z + a->vals[7] * b->w;
    res->z = a->vals[8] * b->x + a->vals[9] * b->y + a->vals[10] * b->z + a->vals[11] * b->w;
    res->w = a->vals[12] * b->x + a->vals[13] * b->y + a->vals[14] * b->z + a->vals[15] * b->w;
}