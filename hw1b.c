#include "resources/scene.h"
#include "resources/image.h"

#include "raytracer/raytracer.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef THREAD_IT
#include <pthread.h>
#include <sys/sysinfo.h>
#endif

//Author: Mason John Hawver

struct patch
{
    unsigned int start, end;
    rtctx* m_scene;

    Image* image;
};

typedef struct patch patch;

void patch_init(int i, int n, rtctx* s, Image* img, patch* p) 
{
    p->m_scene = s;

    unsigned int width = (i == n - 1) ? ceil((float)(s->width * s->height) / (float)n) : floor((float)(s->width * s->height) / (float)n);

    p->start = i * width;
    p->end = (i + 1) * width;

    p->image = img;
}

void *compute_patch(void* arg) 
{
    patch* p_data = (patch*)arg;

    vec2 s_uv;
    ray s_ray;

    int hit;
    float time_hit;
    vec3 s_normal;
    material s_material;

    vec3 s_color;

    for (int i = p_data->start; i < p_data->end; i++) 
    {
        // printf("%d %d\n", i, p_data->image->width * p_data->image->height);
        image_getuv(i, &s_uv, p_data->image); //find uv

        rtctx_init_ray(&s_uv, p_data->m_scene, &s_ray); //make the ray for the uv

        rtctx_cast_ray(&s_ray, p_data->m_scene, &hit, &time_hit, &s_normal, &s_material); // cast the ray find what when and how it hit an object (if it did)
        rtctx_shade(p_data->m_scene, hit, time_hit, &s_normal, &s_material, &s_color); // shade the pixel based on the ray cast outputs

        image_setpixel(i, &s_color, p_data->image); //set pixel color
    }
}

int main(int argc, char* argv[]) 
{

    char *input_fn;
    char *output_fn;
    
    //Check if we have args
    if (argc < 2) { printf("ERROR: no input file specified\n"); return 0; }
    input_fn = argv[1];

    if (argc >= 3) { output_fn = argv[2]; }
    else //make a output_fn is not provided
    {
        int i = strlen(input_fn) - 1;
        while (i >= 0 && input_fn[i] != '/' && input_fn[i] != '.') { i--; }

        if (input_fn[i] == '/' || i < 0) { printf("ERROR: no output file specified and can't deduce one from the input file name\n"); return 0; }
        output_fn = malloc(sizeof(char) * (i + 5));
        strncpy(output_fn, input_fn, i);
        strcat(output_fn, ".ppm");
    }

    //Load scene
    rtctx s_scene;
    {
        //Load raw scene from the specified input file
        scene s_rs;
        char error_msg[256];
        int success = scene_load(input_fn, &s_rs, error_msg);

        //check for parsing errors or missing params
        if (!success) { printf("ERROR: failed to read {%s}; \nWHY: %s\n", input_fn, error_msg); return 1; }
        printf("SCENE {%s} LOADED\n", input_fn);

        //load scene from raw scene, do precomputations for ray tracing
        rtctx_load(&s_rs, &s_scene);
        printf("CTX {bvh built} CREATED\n");
        
        scene_free(&s_rs); //free raw scene, its no longer needed
    }

    //Init image to store output 
    Image img;
    image_init(&img);
    image_loadblank(s_scene.width, s_scene.height, RGB_8, &img);

    // printf("This system has %d processors configured and "
    // "%d processors available.\n",
    // get_nprocs_conf(), get_nprocs());

#ifdef THREAD_IT

    int threads = get_nprocs();
    patch* patches = malloc(sizeof(patch) * threads);
    pthread_t* tids = malloc(sizeof(pthread_t) * threads);

    printf("SYS {%d threads} SPUN UP\n", threads);

    for (int i = 0; i < threads; i++) { patch_init(i, threads, &s_scene, &img, patches + i); }
    for (int i = 0; i < threads; i++) { pthread_create(tids + i, NULL, compute_patch, (void*)(patches + i)); }
    for (int i = 0; i < threads; i++) { pthread_join(tids[i], NULL); }

    free(patches);
    free(tids);

#endif

#ifndef THREAD_IT

    printf("SYS {no threads} SPUN UP\n");

    patch img_patch;
    patch_init(0, 1, &s_scene, &img, &img_patch);
    compute_patch(&img_patch);

#endif

    //Save Image
    image_saveppm(output_fn, &img);
    printf("IMAGE {%s} SAVED\n", output_fn);
    
    //Free Scene / Img
    image_free(&img);
    rtctx_free(&s_scene);

    if (argc < 3) { free(output_fn); }

    // pthread_exit(NULL);
    exit(0);
}