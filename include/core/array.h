#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>

//chuck of data
struct chunk 
{
    char* data;
    unsigned int used;
    struct chunk* next;
};

typedef struct chunk chunk;

//arena allocator for one data type
struct arena 
{
    chunk* head;
    chunk* tail;

    size_t elem_size;
    unsigned int chunk_cap;

    unsigned int size;
};

//iter object for easy and quick itteration
struct arena_iter 
{
    chunk* curr;
    char* last;
    unsigned int index;
};

typedef struct arena arena;
typedef struct arena_iter arena_iter;

//init arena
//chuck capacity {cc} the number of elements in the arena
//element size {es} size of type in bytes
//{arr} ptr to arena
void   arena_init(unsigned int cc, size_t es, arena* arr);

//push back {data} to arena, {arr}
//returns 1 for success and 0 otherwise
void*  arena_push(arena* arr);

void*  arena_array(arena* arr);

//init {iter} at the start of the arena {arr}
//returns 1 if theres a element at the iter and 0 otherwise
int    arena_begin(arena_iter* iter, arena* arr);

//{iter}++
//returns 1 if theres a element at the iter and 0 otherwise
int    arena_next(arena_iter* iter, arena* arr);

//get data, {return value} at the {iter} for arena, {arr}
void*  arena_get(arena_iter* iter, arena* arr);

//free arena {arr}
void   arena_free(arena* arr);

void arena_move(arena* arr, arena* cannibal);


#endif