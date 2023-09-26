#ifndef SCENE_NOISE_H
#define SCENE_NOISE_H

struct SceneNoise
{
    unsigned int width, height; //output image size
    
    unsigned int octaves; //noise params
    float led, gain, freq;
};

typedef struct SceneNoise SceneNoise;

//Init {scene} set all values to default
void SceneNoise_init(SceneNoise* scene);

//Free scene
void SceneNoise_free(SceneNoise* scene);

//Load {scene} from {file_name} returns 1 if succesfull 0 if not
int SceneNoise_load(const char * file_name, SceneNoise* scene);

//Write the {scene} out to {file_name} 
void SceneNoise_save(const char* file_name, SceneNoise* scene);

#endif