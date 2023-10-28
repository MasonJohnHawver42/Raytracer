#include "core/hashmap.h"
#include "core/array.h"

#include <string.h>

void hashmap_init(size_t es, hashmap* hm) 
{
    arena_init(128, es, &hm->values);
    for (int i = 0; i < BUCKETS; i++) 
    {
        arena_init(8, sizeof(entry), hm->buckets + i);
    }
}

unsigned int hashmap_hash(char *key) {
    unsigned int h = 5381;
    char c;
    while ((c = *key++)) {
        h = 33 * h + c;
    }
    return h;
}

unsigned int hashmap_bucket(unsigned int hash) 
{
    return hash % BUCKETS;
}

entry* bucket_get(unsigned int hash, arena* bucket) 
{
    arena_iter iter;
    int cont = arena_begin(&iter, bucket);
    while(cont) 
    {
        entry* ent = (entry*)arena_get(&iter, bucket);

        if (ent->hash == hash) 
        {
            return ent;
        }

        cont = arena_next(&iter, bucket);
    }

    return NULL;
}

void* hashmap_get(char* key, hashmap* hm) 
{
    unsigned int hash = hashmap_hash(key);
    unsigned int bucket_index = hashmap_bucket(hash);

    arena* bucket = hm->buckets + bucket_index;

    entry* ent = bucket_get(hash, bucket);
    if(ent == NULL) 
    {
        return NULL;
    }
    return ent->value;
}


void* hashmap_place(char* key, hashmap* hm) 
{
    unsigned int hash = hashmap_hash(key);
    unsigned int bucket_index = hashmap_bucket(hash);

    arena* bucket = hm->buckets + bucket_index;

    entry* ent = bucket_get(hash, bucket);
    if(ent == NULL) 
    {
        ent = (entry*)arena_push(bucket);
        ent->hash = hash;
        ent->value = arena_push(&hm->values);
        return ent->value;
    }
    return ent->value;
}

void* hashmap_get_hash(unsigned int hash, hashmap* hm) 
{
    unsigned int bucket_index = hashmap_bucket(hash);

    arena* bucket = hm->buckets + bucket_index;

    entry* ent = bucket_get(hash, bucket);
    if(ent == NULL) 
    {
        return NULL;
    }
    return ent->value;
}

void* hashmap_place_hash(unsigned int hash, hashmap* hm) 
{
    unsigned int bucket_index = hashmap_bucket(hash);

    arena* bucket = hm->buckets + bucket_index;

    entry* ent = bucket_get(hash, bucket);
    if(ent == NULL) 
    {
        ent = (entry*)arena_push(bucket);
        ent->hash = hash;
        ent->value = arena_push(&hm->values);
        return ent->value;
    }
    return ent->value;
}

void hashmap_free(hashmap* hm) 
{
    arena_free(&hm->values);
    for (int i = 0; i < BUCKETS; i++) 
    {
        arena_free(hm->buckets + i);
    }
}

void* hashmap_move(hashmap* hm, hashmap* cannibal) 
{
    arena_move(&hm->values, &cannibal->values);

    for (int i = 0; i < BUCKETS; i++) 
    {
        arena_move(&hm->buckets[i], &cannibal->buckets[i]);
    }
}

