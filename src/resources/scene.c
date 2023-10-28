#include "resources/scene.h"
#include "resources/image.h"
#include "math/vector.h"

#include "core/common.h"
#include "core/lexer.h"

#include "core/array.h"
#include "core/hashmap.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_CHUNK_SIZE 256

void material_init(material* mat) 
{
    vec3_init(0.0f, 0.0f, 0.0f, &mat->m_diffuse);
    vec3_init(0.0f, 0.0f, 0.0f, &mat->m_specular);
    mat->ka = 0.0f;
    mat->kd = 0.0f;
    mat->ks = 0.0f;
    mat->n  = 0.0f;
    mat->m_tex_id = 0;
}

void face_init(face* f) 
{
    f->v1 = 0;
    f->v2 = 0;
    f->v3 = 0;

    f->nv1 = 0;
    f->nv2 = 0;
    f->nv3 = 0;

    f->uv1 = 0;
    f->uv2 = 0;
    f->uv3 = 0;
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
    arena_free(&s->m_lights);
    arena_free(&s->m_faces);
    arena_free(&s->m_vertexes);
    arena_free(&s->m_normals);
    arena_free(&s->m_uvs);
    arena_free(&s->m_texture_queue);
    hashmap_free(&s->m_textures);
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
    arena_init(ARENA_CHUNK_SIZE, sizeof(light), &s->m_lights);

    arena_init(ARENA_CHUNK_SIZE, sizeof(face), &s->m_faces);
    arena_init(ARENA_CHUNK_SIZE, sizeof(vec3), &s->m_vertexes);
    arena_init(ARENA_CHUNK_SIZE, sizeof(vec3), &s->m_normals);
    arena_init(ARENA_CHUNK_SIZE, sizeof(vec2), &s->m_uvs);

    //texture stuff
    arena_init(ARENA_CHUNK_SIZE, sizeof(char) * 128, &s->m_texture_queue);
    hashmap_init(sizeof(Image), &s->m_textures);
    unsigned int texture_id = 0;

    s->m_depth_cue.apply = 0;

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
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "texture", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "texture failed; expected whitespace after 'texture'"); free(file_data); return 0; }
        
            read_token(&ctx, &curr);
            if (curr.m_type != WORD) { strcpy(msg, "texture failed; expected whitespace after 'texture'"); free(file_data); return 0; }

            char* dest = (char*)arena_push(&s->m_texture_queue);
            memset(dest, '\0', 128);
            strncpy(dest, ctx.str + curr.m_data.m_word.start, (curr.m_data.m_word.size > 127 ? 127 : curr.m_data.m_word.size));
            texture_id = hashmap_hash(dest);
            mat.m_tex_id = texture_id;
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
            // mat.m_tex_id = texture_id;
            sphere_obj_init(&mat, &s_sphere, &s_sphere_obj);
            sphere_obj* dest = (sphere_obj*)arena_push(&s->m_sphere_objs);
            *dest = s_sphere_obj;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "mtlcolor", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "mtlcolor failed; expected whitespace after 'mtlcolor'"); free(file_data); return 0; }

            int success = read_vec3(&ctx, &mat.m_diffuse, buffer);
            if (!success) { sprintf(msg, "mtlcolor failed on diffuse color; %s", buffer); free(file_data); return 0; }
            if (!valid_color(&mat.m_diffuse)) { sprintf(msg, "mtlcolor failed; 0 <= <%f %f %f> <= 1", mat.m_diffuse.x, mat.m_diffuse.y, mat.m_diffuse.z); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "mtlcolor failed; expected whitespace after diffuse"); free(file_data); return 0; }

            success = read_vec3(&ctx, &mat.m_specular, buffer);
            if (!success) { sprintf(msg, "mtlcolor failed on specular color; %s", buffer); free(file_data); return 0; }
            if (!valid_color(&mat.m_specular)) { sprintf(msg, "mtlcolor failed; 0 <= <%f %f %f> <= 1", mat.m_specular.x, mat.m_specular.y, mat.m_specular.z); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "mtlcolor failed; expected whitespace after specular"); free(file_data); return 0; }

            success = read_float(&ctx, &mat.ka, buffer);
            if (!success) { sprintf(msg, "mtlcolor failed on ka; %s", buffer); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "mtlcolor failed; expected whitespace after ka"); free(file_data); return 0; }

            success = read_float(&ctx, &mat.kd, buffer);
            if (!success) { sprintf(msg, "mtlcolor failed on kd; %s", buffer); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "mtlcolor failed; expected whitespace after kd"); free(file_data); return 0; }

            success = read_float(&ctx, &mat.ks, buffer);
            if (!success) { sprintf(msg, "mtlcolor failed on ks; %s", buffer); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "mtlcolor failed; expected whitespace after ks"); free(file_data); return 0; }

            success = read_float(&ctx, &mat.n, buffer);
            if (!success) { sprintf(msg, "mtlcolor failed on n; %s", buffer); free(file_data); return 0; }
            if (s->v_fov <= 0.0f || s->v_fov >= 180.0f) { sprintf(msg, "vfov failed; 0 < %f < 180", s->v_fov); free(file_data); return 0; }
        
            // mat.m_tex_id = texture_id;
        }
        if (curr.m_type == WORD && (wordtoken_equal(&ctx, "light", &curr) || wordtoken_equal(&ctx, "attlight", &curr))) 
        {
            int att = wordtoken_equal(&ctx, "attlight", &curr) ? 1 : 0; //could be only done once
            
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { sprintf(msg, "%s failed; expected whitespace after 'light'", att ? "attlght" : "light"); free(file_data); return 0; }
        
            vec3 vec1;
            float w;
            
            light pl;

            int success = read_vec3(&ctx, &vec1, buffer);
            if (!success) { sprintf(msg, "%s failed on xyz; %s", att ? "attlght" : "light", buffer); free(file_data); return 0; }
            
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { sprintf(msg, "%s failed; expected whitespace after xyz", att ? "attlght" : "light"); free(file_data); return 0; }

            success = read_float(&ctx, &w, buffer);
            if (!success) { sprintf(msg, "%s failed on w; %s", att ? "attlght" : "light", buffer); free(file_data); return 0; }

            if (w == 1.0f) 
            {
                
                pl.m_type = att ? AT_POINT : POINT;
                pl.m_pos = vec1;

            }
            else if (w == 0.0f) 
            {
                pl.m_type = DIRECTIONAL;
                pl.m_vel = vec1;
            }
            else { sprintf(msg, "%s failed on w; %f == 1 or %f == 0", att ? "attlght" : "light", w, w); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { sprintf(msg, "%s failed; expected whitespace after w", att ? "attlght" : "light"); free(file_data); return 0; }

            success = read_vec3(&ctx, &pl.m_color, buffer);
            if (!success) { sprintf(msg, "%s failed on color; %s", att ? "attlght" : "light", buffer); free(file_data); return 0; }
            if (!valid_color(&pl.m_color)) { sprintf(msg, "%s failed; 0 <= <%f %f %f> <= 1", att ? "attlght" : "light", pl.m_color.x, pl.m_color.y, pl.m_color.z); free(file_data); return 0; }

            if (att) 
            {
                read_token(&ctx, &curr);
                if (curr.m_type != WHITE_SPACE) { sprintf(msg, "%s failed; expected whitespace after color", att ? "attlght" : "light"); free(file_data); return 0; }

                success = read_vec3(&ctx, &pl.m_att, buffer);
                if (!success) { sprintf(msg, "%s failed on att factors; %s", att ? "attlght" : "light", buffer); free(file_data); return 0; }


            }

            light* dest = (light*)arena_push(&s->m_lights);
            *dest = pl;
        }   
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "depthcueing", &curr)) 
        {
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "depthcueing failed; expected whitespace after 'depthcueing'"); free(file_data); return 0; }

            s->m_depth_cue.apply = 1;

            int success = read_vec3(&ctx, &s->m_depth_cue.m_color, buffer);
            if (!success) { sprintf(msg, "depthcueing failed on color; %s", buffer); free(file_data); return 0; }
            if (!valid_color(&s->m_depth_cue.m_color)) { sprintf(msg, "mtlcolor failed; 0 <= <%f %f %f> <= 1", s->m_depth_cue.m_color.x, s->m_depth_cue.m_color.y, s->m_depth_cue.m_color.z); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "depthcueing failed; expected whitespace after 'depthcueing'"); free(file_data); return 0; }

            success = read_float(&ctx, &s->m_depth_cue.a_max, buffer);
            if (!success) { sprintf(msg, "depthcueing failed on a_max; %s", buffer); free(file_data); return 0; }


            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "depthcueing failed; expected whitespace after 'color'"); free(file_data); return 0; }

            success = read_float(&ctx, &s->m_depth_cue.a_min, buffer);
            if (!success) { sprintf(msg, "depthcueing failed on a_min; %s", buffer); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "depthcueing failed; expected whitespace after 'a_min'"); free(file_data); return 0; }

            success = read_float(&ctx, &s->m_depth_cue.d_max, buffer);
            if (!success) { sprintf(msg, "depthcueing failed on d_max; %s", buffer); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "depthcueing failed; expected whitespace after 'd_max'"); free(file_data); return 0; }

            success = read_float(&ctx, &s->m_depth_cue.d_min, buffer);
            if (!success) { sprintf(msg, "depthcueing failed on d_min; %s", buffer); free(file_data); return 0; }
        
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
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "f", &curr)) 
        {

            face s_face;
            face_init(&s_face);

            s_face.mat = mat;
            // s_face.mat.m_tex_id = texture_id;
            
            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "f failed expected whitespace after f"); free(file_data); return 0; }

            // v1          i
            // v1/uv1      i s i    
            // v1//nv1     i s s i
            // v1/uv1/nv1  i s i s i

            read_token(&ctx, &curr);
            if (curr.m_type != INT) { strcpy(msg, "f failed expected int after whitspace"); free(file_data); return 0; }
            if (curr.m_data.m_int <= 0) { sprintf(msg, "f failed expected index %d > 0", curr.m_data.m_int); free(file_data); return 0; }
            s_face.v1 = curr.m_data.m_int;



            read_token(&ctx, &curr); //can be a slash or whitespace 
            if (curr.m_type != SLASH && curr.m_type != WHITE_SPACE) { strcpy(msg, "f failed expected slash or whitspace s i"); return 0; }
            if (curr.m_type == SLASH) 
            {
                read_token(&ctx, &curr); // can be slash or int
                if (curr.m_type != SLASH && curr.m_type != INT) { strcpy(msg, "f failed expected slash or int"); return 0; }
                if (curr.m_type == INT) 
                {
                    if (curr.m_data.m_int <= 0) { sprintf(msg, "f failed expected index %d > 0", curr.m_data.m_int); free(file_data); return 0; }
                    s_face.uv1 = curr.m_data.m_int;
                    read_token(&ctx, &curr); // can be slash or whitespace
                }

                //can be slash or whitespace
                if (curr.m_type != SLASH && curr.m_type != WHITE_SPACE) { strcpy(msg, "f failed expected slash or whitspace"); return 0; }
                if (curr.m_type == SLASH) 
                {
                    read_token(&ctx, &curr); // its an int
                    if (curr.m_type != INT) { strcpy(msg, "f failed expected int"); return 0; }
                    if (curr.m_data.m_int <= 0) { sprintf(msg, "f failed expected index %d > 0", curr.m_data.m_int); free(file_data); return 0; }
                    s_face.nv1 = curr.m_data.m_int;
                    read_token(&ctx, &curr); // has to be a whitespace
                } 
            }

            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "f failed expected whitespace after f"); free(file_data); return 0; }

                        read_token(&ctx, &curr);
            if (curr.m_type != INT) { strcpy(msg, "f failed expected int after whitspace"); free(file_data); return 0; }
            if (curr.m_data.m_int <= 0) { sprintf(msg, "f failed expected index %d > 0", curr.m_data.m_int); free(file_data); return 0; }
            s_face.v2 = curr.m_data.m_int;


            read_token(&ctx, &curr); //can be a slash or whitespace 
            if (curr.m_type != SLASH && curr.m_type != WHITE_SPACE) { strcpy(msg, "f failed expected slash or whitspace s i"); return 0; }
            if (curr.m_type == SLASH) 
            {
                read_token(&ctx, &curr); // can be slash or int
                if (curr.m_type != SLASH && curr.m_type != INT) { strcpy(msg, "f failed expected slash or int"); return 0; }
                if (curr.m_type == INT) 
                {
                    if (curr.m_data.m_int <= 0) { sprintf(msg, "f failed expected index %d > 0", curr.m_data.m_int); free(file_data); return 0; }
                    s_face.uv2 = curr.m_data.m_int;
                    read_token(&ctx, &curr); // can be slash or whitespace
                }

                //can be slash or whitespace
                if (curr.m_type != SLASH && curr.m_type != WHITE_SPACE) { strcpy(msg, "f failed expected slash or whitspace"); return 0; }
                if (curr.m_type == SLASH) 
                {
                    read_token(&ctx, &curr); // its an int
                    if (curr.m_type != INT) { strcpy(msg, "f failed expected int"); return 0; }
                    if (curr.m_data.m_int <= 0) { sprintf(msg, "f failed expected index %d > 0", curr.m_data.m_int); free(file_data); return 0; }
                    s_face.nv2 = curr.m_data.m_int;
                    read_token(&ctx, &curr); // has to be a whitespace
                } 
            }

            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "f failed expected whitespace after f"); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != INT) { strcpy(msg, "f failed expected int after whitspace"); free(file_data); return 0; }
            if (curr.m_data.m_int <= 0) { sprintf(msg, "f failed expected index %d > 0", curr.m_data.m_int); free(file_data); return 0; }
            s_face.v3 = curr.m_data.m_int;


            read_token(&ctx, &curr); //can be a slash or whitespace 
            if (curr.m_type != SLASH && curr.m_type != WHITE_SPACE && curr.m_type != EOF_TOKEN) { strcpy(msg, "f failed expected slash or whitspace s i"); return 0; }
            if (curr.m_type == SLASH) 
            {
                read_token(&ctx, &curr); // can be slash or int
                if (curr.m_type != SLASH && curr.m_type != INT) { strcpy(msg, "f failed expected slash or int"); return 0; }
                if (curr.m_type == INT) 
                {
                    if (curr.m_data.m_int <= 0) { sprintf(msg, "f failed expected index %d > 0", curr.m_data.m_int); free(file_data); return 0; }
                    s_face.uv3 = curr.m_data.m_int;
                    read_token(&ctx, &curr); // can be slash or whitespace
                }

                //can be slash or whitespace
                if (curr.m_type != SLASH && curr.m_type != WHITE_SPACE) { strcpy(msg, "f failed expected slash or whitspace"); return 0; }
                if (curr.m_type == SLASH) 
                {
                    read_token(&ctx, &curr); // its an int
                    if (curr.m_type != INT) { strcpy(msg, "f failed expected int"); return 0; }
                    if (curr.m_data.m_int <= 0) { sprintf(msg, "f failed expected index %d > 0", curr.m_data.m_int); free(file_data); return 0; }
                    s_face.nv3 = curr.m_data.m_int;
                    read_token(&ctx, &curr); // has to be a whitespace
                } 
            }


            // if (curr.m_type != WHITE_SPACE) { strcpy(msg, "f failed expected whitespace after f"); free(file_data); return 0; }

            face* dest = (face*)arena_push(&s->m_faces);
            *dest = s_face;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "v", &curr)) 
        {
            vec3 s_pos;

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "v failed; expected white space after 'v'"); free(file_data); return 0; }

            int success = read_vec3(&ctx, &s_pos, buffer);
            if (!success) { sprintf(msg, "v failed; %s", buffer); free(file_data); return 0; }

            vec3* dest = (vec3*)arena_push(&s->m_vertexes);
            *dest = s_pos;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "vn", &curr)) 
        {
            vec3 s_normal;

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "vn failed; expected white space after 'vn'"); free(file_data); return 0; }

            int success = read_vec3(&ctx, &s_normal, buffer);
            if (!success) { sprintf(msg, "vn failed; %s", buffer); free(file_data); return 0; }

            vec3* dest = (vec3*)arena_push(&s->m_normals);
            *dest = s_normal;
        }
        if (curr.m_type == WORD && wordtoken_equal(&ctx, "vt", &curr)) 
        {
            vec2 uv;

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "vt failed; expected white space after 'vt'"); free(file_data); return 0; }

            int success = read_float(&ctx, &uv.x, buffer);
            if (!success) { sprintf(msg, "vt failed; %s", buffer); free(file_data); return 0; }

            read_token(&ctx, &curr);
            if (curr.m_type != WHITE_SPACE) { strcpy(msg, "vt failed; expected white space after uv.x"); free(file_data); return 0; }

            success = read_float(&ctx, &uv.y, buffer);
            if (!success) { sprintf(msg, "vt failed; %s", buffer); free(file_data); return 0; }
       
            vec2* dest = (vec2*)arena_push(&s->m_uvs);
            *dest = uv;
        }

        //finish other cmds
    } while(curr.m_type != EOF_TOKEN);


    free(file_data);
    if (!imsize_found) { strcpy(msg, "missing imsize"); return 0; }
    if (!eye_found) { strcpy(msg, "missing eye"); return 0; }
    if (!viewdir_found) { strcpy(msg, "missing viewdir"); return 0; }
    if (!updir_found) { strcpy(msg, "missing updir"); return 0; }
    if (!vfov_found) { strcpy(msg, "missing vfov"); return 0; }
    if (!bkgcolor_found) { strcpy(msg, "missing bkgcolor"); return 0; }

    arena_iter iter; 
    int cont = arena_begin(&iter, &s->m_texture_queue);
    while (cont) 
    {
        char* name = (char*)(arena_get(&iter, &s->m_texture_queue));

        Image *dest = (Image*)hashmap_place(name, &s->m_textures);
        image_init(dest);

        char path[256] = "./assets/textures/";
        strcat(path, name);

        int success = image_loadppm(path, dest);
        if (success == 2) 
        {
            strcpy(path, "./hw1c/texture/");
            strcat(path, name);

            success = image_loadppm(path, dest);
        }
        if (success == 2) 
        {
            strcpy(path, "./texture/");
            strcat(path, name);

            success = image_loadppm(path, dest);
        }
        
        if (success == 1) 
        {
            printf("Image {%s} Loaded \n", path);
        }

        cont = arena_next(&iter, &s->m_texture_queue);
    }

    arena_free(&s->m_texture_queue);


    return 1;
}
