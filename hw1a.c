#include "resources/scene.h"
#include "resources/image.h"

#include "raytracer/raytracer.h"

#include <math.h>

//Author: Mason John Hawver

int main(int argc, char* argv[]) 
{

    //Check if we have args
    if (argc < 2) { printf("ERROR: no input file specified\n"); return 0; }
    if (argc < 3) { printf("ERROR: no output file specified\n"); return 0; }

    //Load scene
    rtctx s_scene;
    {
        //Load raw scene from the specified input file
        scene s_rs;
        char error_msg[256];
        int success = scene_load(argv[1], &s_rs, error_msg);

        //check for parsing errors or missing params
        if (!success) { printf("ERROR: failed to read {%s}; \nWHY: %s\n", argv[1], error_msg); return 1; }

        //load scene from raw scene, do precomputations for ray tracing
        rtctx_load(&s_rs, &s_scene);
        printf("SCENE {%s} LOADED\n", argv[1]);
        scene_free(&s_rs); //free raw scene, its no longer needed
    }

    //Init image to store output 
    Image img;
    image_init(&img);
    image_loadblank(s_scene.width, s_scene.height, RGB_8, &img);

    //init variables needed for ray tracing
    vec2 s_uv;
    ray s_ray;

    int hit;
    float time_hit;
    vec3 s_normal;
    material s_material;

    vec3 s_color;

    for (int i = 0; i < s_scene.width * s_scene.height; i++) 
    {
        image_getuv(i, &s_uv, &img); //find uv
        rtctx_init_ray(&s_uv, &s_scene, &s_ray); //make the ray for the uv

        rtctx_cast_ray(&s_ray, &s_scene, &hit, &time_hit, &s_normal, &s_material); // cast the ray find what when and how it hit an object (if it did)
        rtctx_shade(&s_scene, hit, time_hit, &s_normal, &s_material, &s_color); // shade the pixel based on the ray cast outputs

        image_setpixel(i, &s_color, &img); //set pixel color
    }

    //Save Image
    image_saveppm(argv[2], &img);
    printf("IMAGE {%s} SAVED\n", argv[2]);
    
    //Free Scene / Img
    image_free(&img);
    rtctx_free(&s_scene);
}