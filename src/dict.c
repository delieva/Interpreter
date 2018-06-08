#include <stdlib.h>

#include "../include/list.h"
#include "../include/dict.h"
#include "../include/bstree.h"

struct __Dict {
    BSTree * pairs;
};

// http://www.cse.yorku.ca/~oz/hash.html 
int hashString(char *str)
{
   int hash = 5381;
   int c;

   while ((c = (unsigned char)*str++))
       hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

   return hash;
}


Dict * Dict_new(void){
    Dict * self = malloc(sizeof(Dict));
    self->pairs = BSTree_new();
    return self;
}

void Dict_free(Dict * self){
    BSTree_clear(self->pairs);
    BSTree_free(self->pairs);
    free(self);
}


void Dict_add (Dict * self, char * key, void * value){
    int intKey = hashString(key);
    BSTree_insert(self->pairs, intKey, value, key);
}

bool Dict_contains (Dict * self, char * key){
    int intKey = hashString(key);
    return BSTree_lookup(self->pairs, intKey);
}

void * Dict_get (Dict * self, char * key){
    int intKey = hashString(key);
    return BSTree_search(self->pairs, intKey);
}

void * Dict_set (Dict * self, char * key, void * value){
    int intKey = hashString(key);
    return BSTree_set(self->pairs, intKey, value);
}

void * Dict_remove (Dict * self, char * key){
    int intKey = hashString(key);
    return BSTree_delete(self->pairs, intKey);
}

void Dict_clear (Dict * self){
    BSTree_clear(self->pairs);
}

size_t Dict_count (Dict * self){
    return BSTree_count(self->pairs);
}


void Dict_keys (Dict * self, List * keys){
    BSTree_keys(self->pairs, keys);
}

void Dict_values (Dict * self, List * values){
    BSTree_values(self->pairs, values);
}