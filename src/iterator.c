#include "../include/iterator.h"
#include "../include/list.h"

#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

struct __Iterator {
    List * list;
    IteratorDistance index;
};

void Iterator_free(Iterator * self){
    free(self);
} 

void * Iterator_value    (Iterator * self){
    assert(self->index < List_count(self->list));
    return List_at(self->list, self->index);
}

void   Iterator_next     (Iterator * self){
    self->index++;
}

void   Iterator_prev     (Iterator * self){
    self->index--;
}

void   Iterator_advance  (Iterator * self, IteratorDistance n){
    self->index += n;
}

bool   Iterator_equals   (Iterator * self, Iterator * other){
    return self->list == other->list && self->index == other->index;
}

IteratorDistance Iterator_distance (Iterator * begin, Iterator * end){
    return end->index - begin->index;
}