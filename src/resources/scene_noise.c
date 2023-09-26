#include "resources/scene_noise.h"

#include "util/common.h"
#include "util/lexer.h"

#include <stdlib.h>

void SceneNoise_init(SceneNoise* scene) 
{
    scene->width = 0;
    scene->height = 0;
    scene->octaves = 0;
    scene->led = 0;
    scene->gain = 0;
    scene->freq = 0;
}

void SceneNoise_free(SceneNoise* scene) {}

int SceneNoise_load(const char * file_name, SceneNoise* scene) 
{
    int size;
    char* file_data = file_loaddata(file_name, &size);

    Token curr;
    LexerContext ctx;

    lexerctx_init(file_data, size, &ctx);

    int params_found = 0;
    
    do 
    {
        read_token(&ctx, &curr);
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "imsize", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != INT || curr.m_data.m_int <= 0) { return 0; }
            scene->width = curr.m_data.m_int;

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { return 0;  }

            read_token(&ctx, &curr);
            if (curr.m_type != INT || curr.m_data.m_int <= 0) { return 0; }
            scene->height = curr.m_data.m_int;
            params_found++;
        }
        else if(curr.m_type == WORD && wordtoken_equal(&ctx, "noise", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { return 0;  }

            read_token(&ctx, &curr);
            if (curr.m_type != INT || curr.m_data.m_int <= 0) { return 0; }
            scene->octaves = curr.m_data.m_int;

            read_token(&ctx, &curr); //WHITSPACE
            if (curr.m_type != WHITE_SPACE) { return 0;  }

            read_token(&ctx, &curr);
            if (curr.m_type != FLOAT) { return 0;  }
            scene->led = curr.m_data.m_float;

            read_token(&ctx, &curr); //WHITSPACE
            if (curr.m_type != WHITE_SPACE) { return 0;  }

            read_token(&ctx, &curr);
            if (curr.m_type != FLOAT) { return 0;  }
            scene->gain = curr.m_data.m_float;

            read_token(&ctx, &curr); //WHITSPACE
            if (curr.m_type != WHITE_SPACE) { return 0;  }

            read_token(&ctx, &curr);
            if (curr.m_type != FLOAT) { return 0;  }
            scene->freq = curr.m_data.m_float;
            params_found++;
        }

    } while(curr.m_type != EOF_TOKEN);

    free(file_data);

    return params_found == 2;
}

void SceneNoise_save(const char* file_name, SceneNoise* scene) {}
