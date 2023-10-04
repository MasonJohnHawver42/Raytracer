
#include "resources/image.h"

#include "core/lexer.h"
#include "core/common.h"

#include "math/common.h"
#include "math/vector.h"
#include "math/noise.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int get_channels(int format) 
{
    switch(format) 
    {
        case RGB_8: return 3;
        case RG_8: return 2;
        case R_8: return 1;
        default: printf("RESOURCES::ERROR [get_channels : format %i not found]\n", format); return 0;
    }
}

void image_init(Image* image) 
{
    image->data = NULL;
    image->width = 0;
    image->height = 0;
    image->format = 0;
}

void image_free(Image *image) 
{
    if (image->data != NULL) { free(image->data); image->data = NULL; }
    image->width = 0;
    image->height = 0;
    image->format = 0;
}

int image_loadppm(const char* file_name, Image* image) 
{
    image_free(image);
    
    int size;
    char* file_data = file_loaddata(file_name, &size);

    Token curr;
    LexerContext ctx;

    lexerctx_init(file_data, size, &ctx);

    read_token(&ctx, &curr);
    if (curr.m_type != WORD && !wordtoken_equal(&ctx, "P3", &curr)) { printf("RESOURCES::ERROR [image_loadppm : format violated 0]"); return 0; }

    read_token(&ctx, &curr);
    if (curr.m_type != WHITE_SPACE) { printf("RESOURCES::ERROR [image_loadppm : format violated 1]\n"); return 0; }

    read_token(&ctx, &curr);
    if (curr.m_type != INT) { printf("RESOURCES::ERROR [image_loadppm : format violated 2]\n"); return 0; }
    image->width = curr.m_data.m_int;

    read_token(&ctx, &curr);
    if (curr.m_type != WHITE_SPACE) { printf("RESOURCES::ERROR [image_loadppm : format violated 3]\n"); return 0; }

    read_token(&ctx, &curr);
    if (curr.m_type != INT) { printf("RESOURCES::ERROR [image_loadppm : format violated 4]\n"); return 0; }
    image->height = curr.m_data.m_int;

    read_token(&ctx, &curr);
    if (curr.m_type != WHITE_SPACE) { printf("RESOURCES::ERROR [image_loadppm : format violated 5]\n"); return 0; }

    read_token(&ctx, &curr);
    if (curr.m_type != INT || curr.m_data.m_int != 255) { printf("RESOURCES::ERROR [image_loadppm : format violated 6]\n"); return 0; }
    image->format = RGB_8;
    // printf("width: %i\n", image->width);
    // printf("height: %i\n", image->height);
    // printf("format: %i\n", image->format);

    read_token(&ctx, &curr);
    if (curr.m_type != WHITE_SPACE) { printf("RESOURCES::ERROR [image_loadppm : format violated 7]\n"); return 0; }


    int channels = get_channels(image->format);
    image->data = malloc(channels * image->width * image->height);

    int i = 0;
    int pixels_read = 0;
    while (i < channels * image->width * image->height && curr.m_type != EOF_TOKEN) 
    {
        read_token(&ctx, &curr);
        if (curr.m_type == INT) 
        { 
            image->data[i++] = (unsigned char)curr.m_data.m_int; 
            pixels_read++;
            // printf("pixel: %u\n", (unsigned char)curr.m_data.m_int); 
        }
    }

    free(file_data);
    return pixels_read == channels * image->width * image->height;
}

void image_loadblank(unsigned int width, unsigned int height, int format, Image* image) 
{

    int channels = get_channels(format);

    image_free(image);

    image->data = malloc(channels * width * height);
    memset(image->data, 0, channels * width * height * sizeof(char));
    
    image->width = width;
    image->height = height;
    image->format = format;
}

void image_setpixel(unsigned int i, vec3* v, Image* image) 
{
    switch(image->format) 
    {
        case RGB_8:
            image->data[i * 3 + 0] = (unsigned char)(v->x * 255);
            image->data[i * 3 + 1] = (unsigned char)(v->y * 255);
            image->data[i * 3 + 2] = (unsigned char)(v->z * 255); break;
        default:
            printf("RESOURCES::ERROR [image_setpixel : format %i not supported]\n", image->format); return;
    }
}

void image_getpixel(unsigned int i, vec3* v, Image* image) 
{
    switch(image->format) 
    {
        case RGB_8:
            v->x = (float)image->data[i * 3 + 0] / (float)255;
            v->y = (float)image->data[i * 3 + 1] / (float)255;
            v->z = (float)image->data[i * 3 + 2] / (float)255; break;
        default:
            printf("RESOURCES::ERROR [image_getpixel : format not supported]"); return;
    }
}

void image_getuv(unsigned int i, vec2* uv, Image* image) 
{
    uv->x = (float)(i % image->width) / (float)image->width; //0 - 1
    uv->y = (floor(i / image->width) / image->height); //0 - 1
}

unsigned int image_getindex(vec2* uv, Image* image) 
{
    unsigned int x = (unsigned int)floor(uv->x * image->width);
    unsigned int y = floor(uv->y * image->height);
    return x + (y * image->width);
}

void image_gen_static(Image* image) 
{
    int channels = get_channels(image->format);

    srand(0);

    for (int i = 0; i < image->width * image->height * channels; i++) 
    {
        image->data[i] = rand() % 256;
    }
}

void image_gen_islandnoise(unsigned int octaves, float l, float g, float f, float z, Image* image) 
{
    int channels = get_channels(image->format);

    float noise_min = fbm_min(octaves, g, 1.0f, 0.5f);
    float noise_max = fbm_max(octaves, g, 1.0f, -0.5f);
    
    float river_size = .2f;
    float river_deepness = .5f;

    vec3 pixel;
    vec2 uv;

    for (int i = 0; i < image->width * image->height; i++) 
    {
        image_getuv(i, &uv, image);
        uv.x *= (float)image->width / (float)image->height; //correct aspect ratio

        float river = fabs(perlin_3d(uv.x * f * l, uv.y * f * l, z * l + 0.0f) * 2.0f);

        river = (river < river_size) ? (river_size - river) * (river_deepness / river_size) : 0.0f;

        float height = fbm(octaves, l, g, 1.0f, f, uv.x, uv.y, z, &perlin_3d);
        float h_norm = (height - noise_min) / (noise_max - noise_min);
        height = 2.0f * (h_norm - 0.5f);
        height = lerp(fade(h_norm), height, (height >= river_deepness) ? height : height - river);

        clamp(height, 0.0f, 1.0f);
        vec3_fill(height, &pixel);
        image_setpixel(i, &pixel, image);
    }

}

void image_mix(Image* dest, Image* source) 
{
    int channels = get_channels(dest->format);
    int n1 = dest->width * dest->height;
    int n2 = source->width * source->height;

    vec3 a, b, r;
    vec2 uv;

    for (unsigned int i = 0; i < n1; i++) 
    {
        image_getuv(i, &uv, dest);
        uv.x *= (float)dest->width / (float)dest->height;

        if (uv.x <= 1.0f) 
        {
            unsigned int j = image_getindex(&uv, source);
            image_getpixel(i, &a, dest);
            image_getpixel(j, &b, source);
            if(b.x > .95f) 
            {
                vec3_init(a.x, a.y, a.z, &r);
            }
            else
            {
                vec3_init(lerp(.2f, 0.5f, 1 - a.x), 
                          lerp(.2f, 0.5f, 1 - a.y), 
                          lerp(.2f, 1.0f, a.z), &r);
            }
            image_setpixel(i, &r, dest);
        }
    }
}

void image_saveppm(const char* file_name, Image* image) 
{
    FILE *out_file = fopen(file_name, "w");

    if (!out_file) 
    {
        printf("RESOURCES::ERROR [save_imageppm : out file not found]");
        return; 
    }

    fprintf(out_file, "P3 \n%d %d \n255\n", image->width, image->height);

    int channels = get_channels(image->format);

    for (int i = 0; i < image->width * image->height * channels; i++) 
    {
        fprintf(out_file, "%u ", image->data[i]);
        if ((i + 1) % 40 == 0) { fprintf(out_file, "\n"); }
    }

    fclose(out_file);
}