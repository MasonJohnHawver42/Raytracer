
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "resources/image.h"
#include "math/vector.h"

#include <math.h>

int main(int argc, char* argv[]) 
{

    if (argc < 2) { printf("ERROR: no img_a file specified\n"); return 0; }
    if (argc < 3) { printf("ERROR: no img_b file specified\n"); return 0; }
    if (argc < 4) { printf("ERROR: no img_diff file specified\n"); return 0; }

    Image img_a;
    image_init(&img_a);
    int success = image_loadppm(argv[1], &img_a);

    if (!success) { printf("IMAGE {%s} FAILED to LOAD\n", argv[1]); return 1; }    

    Image img_b;
    image_init(&img_b);
    success = image_loadppm(argv[2], &img_b);

    if (!success) { printf("IMAGE {%s} FAILED to LOAD\n", argv[2]); return 1; }    

    if (img_a.width != img_b.width || img_a.height != img_b.height) 
    {
        printf("IMAGE sizes dont match\n"); return 1;
    }

    Image img_diff;
    image_init(&img_diff);
    image_loadblank(img_a.width, img_a.height, RGB_8, &img_diff);

    float diff = 0;

    for (int i = 0; i < img_a.width * img_a.height; i++) 
    {
        vec3 a, b, c;
        image_getpixel(i, &a, &img_a);
        image_getpixel(i, &b, &img_b);

        c.x = (a.x - b.x) * (a.x - b.x);
        c.y = (a.y - b.y) * (a.y - b.y);
        c.z = (a.z - b.z) * (a.z - b.z);

        image_setpixel(i, &c, &img_diff);

        diff += c.x + c.y + c.z;
    }

    printf("DIFF %f / %d = %f\n", diff, img_a.width * img_a.height * 3, diff / (float)(img_a.width * img_a.height * 3));

    image_saveppm(argv[3], &img_diff);

    //Free Scene / Imgs
    image_free(&img_a);
    image_free(&img_b);
    image_free(&img_diff);

    return 0;
}
