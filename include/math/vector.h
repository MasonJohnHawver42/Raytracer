#ifndef VECTOR_H
#define VECTOR_H

struct vec4 { float w, x, y, z; };
struct vec3 { 
    union {
        struct { float x, y, z; };
        float raw[3];
    };
};
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
void  vec3_inv(vec3* a, vec3* b);
void  vec3_mult(vec3* a, vec3* b);

void vec3_bc_mix(vec2* bc, vec3* v0, vec3* v1, vec3* v2, vec3* res);
void vec2_bc_mix(vec2* bc, vec2* v0, vec2* v1, vec2* v2, vec2* res);

void vec3_lerp(float t, vec3* a, vec3* b, vec3* res);
void vec3_fill(float a, vec3* v);

void vec4_init(float x, float y, float z, float w, vec4* v);
void vec4_print(vec4 *v);

#endif