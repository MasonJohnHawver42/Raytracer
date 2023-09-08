#ifndef VECTOR_H
#define VECTOR_H

struct vec3 { float x, y, z; };
struct vec2 { float x, y; };

typedef struct vec3 vec3;
typedef struct vec2 vec2;

void vec3_lerp(float t, vec3* a, vec3* b, vec3* res);
void vec3_setall(float a, vec3* v);
void vec3_set(float x, float y, float z, vec3* v);

#endif