#include "resources/scene.h"
#include "math/vector.h"

#include "core/common.h"
#include "core/lexer.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_CHUNK_SIZE 256

void material_init(material* mat) 
{
    vec3_init(0.0f, 0.0f, 0.0f, &mat->m_color);
}

void sphere_obj_init(material* mat, sphere* sphere, sphere_obj* obj) 
{
    obj->m_mat = *mat;
    obj->m_sphere = *sphere;
}

int read_float(LexerContext* ctx, float* f, char* msg) 
{
    Token curr;
    read_token(ctx, &curr);
    if (curr.m_type == FLOAT) { *f = curr.m_data.m_float; return 1; } 
    if (curr.m_type == INT) { *f = (float)curr.m_data.m_int; return 1; } 
    strcpy(msg, "expected type float or int"); return 0;
}

int read_vec3(LexerContext* ctx, vec3* v, char* msg) 
{
    Token curr;
    int success;
    char buffer[64];
    success = read_float(ctx, &v->x, buffer);
    if (!success) { sprintf(msg, "1st component, %s", buffer); return 0; }

    read_token(ctx, &curr);
    if (curr.m_type != WHITE_SPACE) { strcpy(msg, "expected whitespace after first arg"); return 0;  }

    success = read_float(ctx, &v->y, buffer);
    if (!success) { sprintf(msg, "2nd component, %s", buffer); return 0; }

    read_token(ctx, &curr);
    if (curr.m_type != WHITE_SPACE) { strcpy(msg, "expected whitespace after second arg"); return 0;  }

    success = read_float(ctx, &v->z, buffer);
    if (!success) { sprintf(msg, "3rd component, %s", buffer); return 0; }

    return 1;
}

int valid_color(vec3* v) { return (v->x >= 0.0 && v->x <= 1.0f && v->y >= 0.0 && v->y <= 1.0f && v->z >= 0.0 && v->z <= 1.0f) ? 1 : 0; }

void scene_free(scene *s) 
{
    arena_free(&s->m_sphere_objs);
}

int scene_load(char* fn, scene* s, char* msg) 
{
    int size;
    char* file_data = file_loaddata(fn, &size);

    if (file_data == NULL) { sprintf(msg, "couldn't open file"); return 0; }

    Token curr;
    LexerContext ctx;

    lexerctx_init(file_data, size, &ctx);

    int imsize_found = 0;
    int eye_found = 0;
    int viewdir_found = 0;
    int updir_found = 0;
    int vfov_found = 0;
    int bkgcolor_found = 0;

    char buffer[128];

    material mat;
    material_init(&mat);

    arena_init(ARENA_CHUNK_SIZE, sizeof(sphere_obj), &s->m_sphere_objs);

    do 
    {
        read_token(&ctx, &curr);
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "imsize", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "imsize failed"); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != INT || curr.m_data.m_int <= 0) { strcpy(msg, "imsize failed x < 0 or not int"); free(file_data); return 0; }
            s->width = curr.m_data.m_int;

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "imsize failed"); free(file_data); return 0;  }

            read_token(&ctx, &curr);
            if (curr.m_type != INT || curr.m_data.m_int <= 0) { strcpy(msg, "imsize failed y < 0 or not int"); free(file_data); return 0; }
            s->height = curr.m_data.m_int;
            imsize_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "sphere", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "sphere failed; expected whitespace after 'sphere'"); free(file_data); return 0; }

            sphere s_sphere;

            int success = read_vec3(&ctx, &s_sphere.m_pos, buffer);
            if (!success) { sprintf(msg, "sphere failed; position, %s", buffer); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "sphere failed; expected whitespace after position"); free(file_data); return 0;  }

            success = read_float(&ctx, &s_sphere.m_rad, buffer);
            if (!success) { sprintf(msg, "sphere failed; radius, %s", buffer); free(file_data); return 0; }
            if (s_sphere.m_rad <= 0.0) { sprintf(msg, "sphere failed; radius, 0 < %f", s_sphere.m_rad); free(file_data); return 0;  }

            sphere_obj s_sphere_obj;
            sphere_obj_init(&mat, &s_sphere, &s_sphere_obj);
            arena_push(&s_sphere_obj, &s->m_sphere_objs);
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "mtlcolor", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "mtlcolor failed; expected whitespace after 'mtlcolor'"); free(file_data); return 0; }

            int success = read_vec3(&ctx, &mat.m_color, buffer);
            if (!success) { sprintf(msg, "mtlcolor failed; %s", buffer); free(file_data); return 0; }
            if (!valid_color(&mat.m_color)) { sprintf(msg, "mtlcolor failed; 0 <= <%f %f %f> <= 1", mat.m_color.x, mat.m_color.y, mat.m_color.z); free(file_data); return 0; }

        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "eye", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "eye failed; expected white space after 'eye'"); free(file_data); return 0; }

            int success = read_vec3(&ctx, &s->pos, buffer);
            if (!success) { sprintf(msg, "eye failed; %s", buffer); free(file_data); return 0; }

            eye_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "viewdir", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "viewdir failed; expected whitespace after 'viewdir'"); free(file_data); return 0; }

            int success = read_vec3(&ctx, &s->fwd, buffer);
            if (!success) { sprintf(msg, "viewdir failed; %s", buffer); free(file_data); return 0; }
            if (s->fwd.x == 0 && s->fwd.y == 0 && s->fwd.z == 0) { sprintf(msg, "viewdir failed; <%f %f %f> != <0, 0, 0>", s->fwd.x, s->fwd.y, s->fwd.z); free(file_data); return 0; }

            viewdir_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "updir", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "updir failed; expected whitespace after 'updir'"); return 0; }

            int success = read_vec3(&ctx, &s->up, buffer);
            if (!success) { sprintf(msg, "updir failed; %s", buffer); free(file_data); return 0; }
            if (s->up.x == 0 && s->up.y == 0 && s->up.z == 0) { sprintf(msg, "updir failed; <%f %f %f> != <0, 0, 0>", s->up.x, s->up.y, s->up.z); free(file_data); return 0; }

            updir_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "bkgcolor", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "bkgcolor failed; expected whitespace after 'bkgcolor'"); free(file_data); return 0; }

            int success = read_vec3(&ctx, &s->bg_color, buffer);
            if (!success) { sprintf(msg, "bkgcolor failed; %s", buffer); free(file_data); return 0; }
            if (!valid_color(&s->bg_color)) { sprintf(msg, "bkgcolor failed; 0 <= <%f %f %f> <= 1", s->bg_color.x, s->bg_color.y, s->bg_color.z); free(file_data); return 0; }

            bkgcolor_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "vfov", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "vfov failed"); free(file_data); return 0; }

            int success = read_float(&ctx, &s->v_fov, buffer);
            if (!success) { sprintf(msg, "vfov failed; %s", buffer); free(file_data); return 0; }
            if (s->v_fov <= 0.0f || s->v_fov >= 180.0f) { sprintf(msg, "vfov failed; 0 < %f < 180", s->v_fov); free(file_data); return 0; }

            vfov_found++;
        }
        //finish other cmds
    } while(curr.m_type != EOF_TOKEN);

    free(file_data);
    int success = imsize_found && eye_found && viewdir_found && updir_found && vfov_found && bkgcolor_found ? 1 : 0;
    if (!success) { strcpy(msg, "missing parameter"); }
    return success;
}
