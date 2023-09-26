
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "resources/scene_noise.h"
#include "resources/image.h"

int main() 
{
    //Load Scene
    SceneNoise scene;
    SceneNoise_init(&scene);
    int success = SceneNoise_load("assets/hw0.in", &scene);

    if (!success) { printf("SCENE {%s} FAILED to LOAD\n", "assets/hw0.in"); return 1; }

    // printf("SCENE {%s} LOADED\n\timsize %i, %i\n\tnoise %i, %f, %f, %f\n", "assets/hw0.in", scene.width, scene.height, scene.octaves, scene.led, scene.gain, scene.freq);
    printf("SCENE {%s} LOADED\n", "assets/hw0.in");


    srand(time(NULL));

    //Load Water Mark Image
    Image initials;
    image_init(&initials);
    success = image_loadppm("assets/initials.ppm", &initials);

    if (!success) { printf("IMAGE {%s} FAILED to LOAD\n", "assets/initials.ppm"); return 1; }    
    //printf("IMAGE {%s} LOADED\n\t%ix%i f: %i\n", "assets/initials.ppm", initials.width, initials.height, initials.format);
    printf("IMAGE {%s} LOADED\n", "assets/initials.ppm");


    //Generate Noise Image
    Image img;
    image_init(&img);
    image_loadblank(scene.width, scene.height, RGB_8, &img);
    image_gen_islandnoise(scene.octaves, scene.led, scene.gain, scene.freq, (float)(rand() % (256 * 100)) / 100.0f, &img);
    
    //Apply Watermark
    image_mix(&img, &initials);

    //Save Image
    image_saveppm("out/hw0.ppm", &img);

    printf("IMAGE {%s} SAVED\n", "assets/hw0.ppm");
    
    //Free Scene / Imgs
    image_free(&initials);
    image_free(&img);
    SceneNoise_free(&scene);

    return 0;
}
