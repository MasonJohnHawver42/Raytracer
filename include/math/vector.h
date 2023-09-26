#ifndef VECTOR_H
#define VECTOR_H

struct vec4 { float w, x, y, z; };
struct vec3 { float x, y, z; };
struct vec2 { float x, y; };

typedef struct vec4 vec4;
typedef struct vec3 vec3;
typedef struct vec2 vec2;

void vec2_init(float x, float y, vec2* v);

void vec3_init(float x, float y, float z, vec3* v);
// void vec3_cpy(vec3* other, vec3* v);

//look at code for documentation
void  vec3_add(vec3* a, vec3* b, vec3* res);
void  vec3_sub(vec3* a, vec3* b, vec3* res);
void  vec3_scaled_add(float as, vec3* a, float bs, vec3* b, vec3* res);
float vec3_dot(vec3* a, vec3* b);
void  vec3_cross(vec3* a, vec3* b, vec3* res);
void  vec3_norm(vec3* v);
void  vec3_scale(float s, vec3* v);
void  vec3_print(vec3 *v);

void vec3_lerp(float t, vec3* a, vec3* b, vec3* res);
void vec3_fill(float a, vec3* v);

void vec4_init(float x, float y, float z, float w, vec4* v);
void vec4_print(vec4 *v);

#endif