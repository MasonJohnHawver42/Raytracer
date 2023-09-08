#ifndef NOISE_H
#define NOISE_H

//PERLIN NOISE
float random_grad(int hash, float x, float y, float z);
float perlin_3d(float x, float y, float z);

//FBM
float fbm_max(unsigned int octaves, float g, float amp, float noise_max);
float fbm_min(unsigned int octaves, float g, float amp, float noise_min);
float fbm(unsigned int octaves, float l, float g, float amp, float freq, float x, float y, float z, float (*noise)(float, float, float));

#endif