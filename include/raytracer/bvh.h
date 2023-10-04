#ifndef BVH_H
#define BVH_H

#include "core/array.h"
#include "math/geometry.h"
#include "resources/scene.h"

struct bvh_node 
{
    aabb bounds;
    struct bvh_node* left;
    struct bvh_node* right;

    int is_leaf; //0 false 1 true
    arena sphere_objs;
};

typedef struct bvh_node bvh_node;

struct bvh 
{
    bvh_node* root;
    unsigned int depth;
};

typedef struct bvh bvh;

void bvh_init(bvh* b);

void bvh_build(arena* spheres, bvh* b);

void bvh_free(bvh* b);


#endif