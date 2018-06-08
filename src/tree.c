#include "../include/tree.h"
#include "../include/list.h"

Tree * Tree_new(void * value){
    Tree * self = malloc(sizeof(Tree));
    self->value = value;
    self->children = List_new();
    return self;
}

void Tree_free(Tree * self){
    List_free(self->children);
    free(self);
}
