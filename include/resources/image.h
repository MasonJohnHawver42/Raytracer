#ifndef IMAGE_H
#define IMAGE_H

#include "math/vector.h"

enum PixelFormat_
{
    RGB_8 = 1, //3 channels - 24bpp
    RG_8, //2 channels - 16bpp
    R_8, //1 channel - 8bpp
    RGBA_8, //4 channels - 32bpp (NOT Implimented yet) 
};

typedef int PixelFormat_;

//helper func {format} is PixelFormat
int get_channels(int format);

struct Image 
{
    unsigned char* data; //array of data
    unsigned int width, height; int format; //size
};

typedef struct Image Image;

//Init {image} 
void image_init(Image* image);

//free {image}
void image_free(Image *image) ;

//allocate memory for {image} fill with data from {file_name} and free any previous data
int image_loadppm(const char* file_name, Image* image);

// allocate memory for {image} fill all pixels with 0 and free any previous data
void image_loadblank(unsigned int width, unsigned int height, int format, Image* image);

// set pixel at {i} (i = pixel_x + pixel_y * width) to {v} (v's componets should be between 0-1) in {image}
void image_setpixel(unsigned int i, vec3* v, Image* image);

void image_setpixel_safe(unsigned int i, vec3* v, Image* image);

// get pixel at {i} (i = pixel_x + pixel_y * width) put data into {v} (v's componets should be between 0-1) in {image}
void image_getpixel(unsigned int i, vec3* v, Image* image);

// get uv coord at {i} in {image}
void image_getuv(unsigned int i, vec2* uv, Image* image);

// get index of pixel from {uv} in {image}
unsigned int image_getindex(vec2* uv, Image* image);

// fills {image} with static
void image_gen_static(Image* image);

// fills {image} with a modified perlin / fbm noise
void image_gen_islandnoise(unsigned int octaves, float l, float g, float f, float z, Image* image);

// mixes {dest} and {source} saving result in {dest}
void image_mix(Image* dest, Image* source);

//save {image} to to {file_name}
void image_saveppm(const char* file_name, Image* image);

#endif



