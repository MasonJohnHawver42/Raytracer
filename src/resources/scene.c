#include "resources/scene.h"
#include "math/vector.h"

#include "util/common.h"
#include "util/lexer.h"

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

// void camera_init(camera* c) 
// {
//     vec3_init(10.0f, 0.0f, 0.0f, &c->m_pos);
//     vec3_init(-1.0f, 0.0f, 1.0f, &c->m_dir);
//     vec3_init(0.0f, 1.0f, 0.0f, &c->m_up);
// }

// void scene_init(scene* s) 
// {
//     s->width = 600;
//     s->height = 400;
//     vec3_init(0.0f, 0.0f, 0.0f, &s->background_color);
//     camera_init(&s->m_camera);
//     arena_init(256, sizeof(sphere_obj), &s->m_sphere_objs);
// }


int read_vec3(LexerContext* ctx, vec3* v) 
{
    Token curr;
    read_token(ctx, &curr);
    if (curr.m_type != FLOAT) { return 0; }
    v->x = curr.m_data.m_float;

    read_token(ctx, &curr);
    if (curr.m_type != WHITE_SPACE) { return 0;  }

    read_token(ctx, &curr);
    if (curr.m_type != FLOAT) { return 0; }
    v->y = curr.m_data.m_float;

    read_token(ctx, &curr);
    if (curr.m_type != WHITE_SPACE) { return 0;  }

    read_token(ctx, &curr);
    if (curr.m_type != FLOAT) { return 0; }
    v->z = curr.m_data.m_float;

    return 1;
}

int valid_color(vec3* v) { return (v->x >= 0.0 && v->x <= 1.0f && v->y >= 0.0 && v->y <= 1.0f && v->z >= 0.0 && v->z <= 1.0f) ? 1 : 0; }

void raw_scene_free(raw_scene *s) 
{
    arena_free(&s->m_sphere_objs);
}

int raw_scene_load(char* fn, raw_scene* s, char* msg) 
{
    int size;
    char* file_data = file_loaddata(fn, &size);

    Token curr;
    LexerContext ctx;

    lexerctx_init(file_data, size, &ctx);

    int imsize_found = 0;
    int eye_found = 0;
    int viewdir_found = 0;
    int updir_found = 0;
    int vfov_found = 0;
    int bkgcolor_found = 0;

    material mat;
    material_init(&mat);

    arena_init(ARENA_CHUNK_SIZE, sizeof(sphere_obj), &s->m_sphere_objs);

    do 
    {
        read_token(&ctx, &curr);
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "imsize", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "imsize failed"); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != INT || curr.m_data.m_int <= 0) { strcpy(msg, "imsize failed x < 0 or not int"); return 0; }
            s->width = curr.m_data.m_int;

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "imsize failed"); return 0;  }

            read_token(&ctx, &curr);
            if (curr.m_type != INT || curr.m_data.m_int <= 0) { strcpy(msg, "imsize failed y < 0 or not int"); return 0; }
            s->height = curr.m_data.m_int;
            imsize_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "sphere", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "sphere failed"); return 0; }

            sphere s_sphere;

            if (!read_vec3(&ctx, &s_sphere.m_pos)) { strcpy(msg, "sphere failed"); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "sphere failed"); return 0;  }

            read_token(&ctx, &curr);
            if (curr.m_type != FLOAT || curr.m_data.m_float <= 0) { strcpy(msg, "sphere failed"); return 0; }
            s_sphere.m_rad = curr.m_data.m_float;

            sphere_obj s_sphere_obj;
            sphere_obj_init(&mat, &s_sphere, &s_sphere_obj);
            arena_push(&s_sphere_obj, &s->m_sphere_objs);
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "mtlcolor", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "mtlcolor failed"); return 0; }

            if (!read_vec3(&ctx, &mat.m_color) || !valid_color(&mat.m_color)) { strcpy(msg, "mtlcolor failed"); return 0; }

            bkgcolor_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "eye", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "eye failed"); return 0; }

            if (!read_vec3(&ctx, &s->pos)) { strcpy(msg, "eye failed"); return 0; }

            eye_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "viewdir", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "viewdir failed"); return 0; }

            if (!read_vec3(&ctx, &s->fwd) || (s->fwd.x == 0 && s->fwd.y == 0 && s->fwd.z == 0)) { strcpy(msg, "viewdir failed"); return 0; }

            viewdir_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "updir", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "updir failed"); return 0; }

            if (!read_vec3(&ctx, &s->up) || (s->up.x == 0 && s->up.y == 0 && s->up.z == 0)) { strcpy(msg, "updir failed"); return 0; }

            updir_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "bkgcolor", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "bkgcolor failed");  return 0; }

            if (!read_vec3(&ctx, &s->bg_color) || !valid_color(&s->bg_color)) { strcpy(msg, "bkgcolor failed"); return 0; }

            bkgcolor_found++;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "vfov", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "vfov failed"); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != FLOAT || curr.m_data.m_float <= 0.0f || curr.m_data.m_float >= 180.0f) { strcpy(msg, "vfov failed"); return 0; }
            s->v_fov = curr.m_data.m_float;

            vfov_found++;
        }
        //finish other cmds
    } while(curr.m_type != EOF_TOKEN);

    free(file_data);
    int success = imsize_found && eye_found && viewdir_found && updir_found && vfov_found && bkgcolor_found ? 1 : 0;
    if (!success) { strcpy(msg, "missing parameter"); }
    return success;
}

void scene_load(raw_scene* raw_s, scene* s) 
{
    s->width = raw_s->width;
    s->height = raw_s->height;
    s->bg_color = raw_s->bg_color;
    s->v_fov = raw_s->v_fov;
    s->m_sphere_objs = raw_s->m_sphere_objs;
    raw_s->m_sphere_objs.head = NULL;
    raw_s->m_sphere_objs.tail = NULL;

    s->m_camera.m_pos = raw_s->pos;
    s->m_camera.m_fwd = raw_s->fwd;
    vec3_cross(&raw_s->fwd, &raw_s->up, &s->m_camera.m_right);
    vec3_cross(&s->m_camera.m_right, &raw_s->fwd, &s->m_camera.m_up);
    vec3_norm(&s->m_camera.m_fwd);
    vec3_norm(&s->m_camera.m_up);
    vec3_norm(&s->m_camera.m_right);

        // vec3_print(&s->m_camera.m_fwd);
        // vec3_print(&s->m_camera.m_up);
        // vec3_print(&s->m_camera.m_right);

    s->scale_x = tan(raw_s->v_fov * (M_PI / 360.0f));
    s->scale_y = s->scale_x * ((float)raw_s->height / (float)raw_s->width);
}

void scene_init_ray(vec2* uv, scene* s, ray* r) 
{
    r->m_pos = s->m_camera.m_pos;

    float xk = ((uv->x * 2.0f) - 1) * s->scale_x;
    float yk = (1 - (2 * uv->y)) * s->scale_y;

    vec3_scaled_add(xk, &s->m_camera.m_right, 1.0f, &s->m_camera.m_fwd, &r->m_vel); 
    vec3_scaled_add(yk, &s->m_camera.m_up, 1.0f, &r->m_vel, &r->m_vel);
    vec3_norm(&r->m_vel); //norm it
}

void scene_cast_ray(ray* s_ray, scene* s, int* hit, float* time_hit, vec3* normal, material* mat) 
{
    //iter through spheres
    arena_iter iter;

    int count;
    float t0, t1;

    *hit = 0;
    sphere_obj* so_hit;

    int cont = arena_begin(&iter, &s->m_sphere_objs);
    while(cont) 
    {
        sphere_obj* s_sphere_obj = (sphere_obj*)arena_get(&iter, &s->m_sphere_objs);
            
        sphere_ray_intersect(s_ray, &s_sphere_obj->m_sphere, &count, &t0, &t1);
            
        if (count > 0) 
        {
            if (t0 < 0.0f) { t0 = t1; }
            if (t0 >= 0.0f && !(*hit && !(t0 < *time_hit))) 
            {
                *hit = 1;
                *time_hit = t0;
                so_hit = s_sphere_obj;
            }
        }

        cont = arena_next(&iter, &s->m_sphere_objs);
    }

    if (*hit) 
    {
        *mat = so_hit->m_mat;

        //calc hit pos and normal
        vec3_scaled_add(1.0f, &s_ray->m_pos, *time_hit, &s_ray->m_vel, normal);
        vec3_sub(normal, &so_hit->m_sphere.m_pos, normal);
        vec3_scale(1.0f / so_hit->m_sphere.m_rad, normal);
    }
}

void scene_shade(scene* s, int hit, float time_hit, vec3* normal, material* mat, vec3* color) 
{
    if (hit) 
    {
        *color = mat->m_color;
    }
    else 
    {
        *color = s->bg_color;
    }
}

void scene_free(scene *s) 
{
    arena_free(&s->m_sphere_objs);
}
