#include "raytracer/bvh.h"

#include "core/array.h"
#include "math/geometry.h"
#include "resources/scene.h"

#include <stdlib.h>

void bvh_init(bvh* b) 
{
    b->root = malloc(sizeof(bvh_node));
    b->depth = 0;
}

void bvh_build(arena* spheres, bvh* b) 
{
    
}

void bvh_free(bvh* b);