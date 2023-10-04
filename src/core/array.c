#include "core/array.h"

// #include <stdio.h>
#include <stdlib.h>
#include <string.h>

void arena_init(unsigned int cc, size_t es, arena* arr) 
{
    arr->head = NULL;
    arr->tail = NULL;
    arr->elem_size = es;
    arr->chunk_cap = cc;
    arr->size = 0;
}

int arena_push(void* data, arena* arr) 
{
    if (!arr->tail) 
    {
        if (arr->head) { return 1; }
        arr->head = malloc(sizeof(chunk));
        if (!arr->head) { return 0; }
        arr->head->data = malloc(arr->elem_size * (arr->chunk_cap));
        if (!arr->head->data) { return 0; }
        arr->head->used = 0;
        arr->head->next = NULL;
        arr->tail = arr->head;
        // printf("block made %i\n", sizeof(chunk) + arr->elem_size * (arr->chunk_cap));
    }

    if (arr->tail->used == arr->chunk_cap) 
    {
        chunk* new_chunk = malloc(sizeof(chunk));
        if (!new_chunk) { return 0; }
        new_chunk->data = malloc(arr->elem_size * (arr->chunk_cap));
        if (!new_chunk->data) { return 0; }
        new_chunk->used = 0;
        new_chunk->next = NULL;
        if (arr->tail->next) { return 1; }
        arr->tail->next = new_chunk;
        arr->tail = new_chunk;
        // printf("block made\n");
    }

    memcpy(arr->tail->data + (arr->tail->used * arr->elem_size), data, arr->elem_size);
    arr->tail->used++;
    arr->size++;

    return 2;
}

int arena_begin(arena_iter* iter, arena* arr) 
{
    if (!arr->head) { return 0; }
    iter->curr = arr->head;
    iter->last = arr->head->data;
    iter->index = 0;
    return 2;
}

int arena_next(arena_iter* iter, arena* arr) 
{
    if (!iter || !arr || !iter->curr) { return 0; }
    if (iter->index + 1 == iter->curr->used) 
    {
        iter->curr = iter->curr->next;
        if (!iter->curr) { return 0; }
        iter->last = iter->curr->data;
        iter->index = 0;
        return 1;
    }

    iter->last = iter->last + arr->elem_size;
    iter->index++;
    return 1;
}

void* arena_get(arena_iter* iter, arena* arr) 
{
    return iter->last;
}

void chunk_free(chunk *curr) 
{
    if (!curr->next) { free(curr); return; }
    chunk_free(curr->next);
    free(curr);
}

void arena_free(arena* arr) 
{
    chunk* curr = arr->head;
    if (!curr) { return; }

    while(curr) 
    {
        free(curr->data);
        curr = curr->next;
    }

    chunk_free(arr->head);
}