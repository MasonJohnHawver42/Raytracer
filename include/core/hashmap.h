#ifndef HASHMAP_H
#define HASHMAP_H

#include "core/array.h"

#define BUCKETS 128

struct entry
{
    unsigned int hash;
    void* value;
};

typedef struct entry entry;

struct hashmap 
{
    arena buckets[BUCKETS];
    arena values;
};

typedef struct hashmap hashmap;

void hashmap_init(size_t es, hashmap* hm);

unsigned int hashmap_hash(char *key);

void* hashmap_get(char* key, hashmap* hm);

void* hashmap_place(char* key, hashmap* hm);

void* hashmap_get_hash(unsigned int hash, hashmap* hm);

void* hashmap_place_hash(unsigned int hash, hashmap* hm);

void* hashmap_move(hashmap* hm, hashmap* cannibal);

void hashmap_free(hashmap* hm);

#endif