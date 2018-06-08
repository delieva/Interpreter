#include "../include/ast.h"
#include "../include/tree.h"
#include "../include/list.h"

#include <stdlib.h>
#include <string.h>

AstNode * AstNode_new(AstNodeType type, char * name, size_t line){
    AstNode * self = malloc(sizeof(AstNode));
    self->name = name;
    self->type = type;
    self->line = line;
    return self;
}

void AstNode_free(AstNode * self){
    if(self->name) free(self->name);
    free(self);
}

void AstTree_free(Tree * self){
    int count = List_count(self->children);
    for(int i = 0; i < count; i++){
        AstTree_free(List_at(self->children, i));
    }
    AstNode_free(self->value);
    Tree_free(self);
}

bool AstNode_equals(AstNode * self, AstNode * other){
    return self->type == other->type && strcmp(self->name, other->name) == 0;
}

bool AstTree_equals(Tree * self, Tree * other){
    int count = List_count(self->children);
    int countOther = List_count(other->children);
    if(count != countOther) return false;
    bool res = true;
    for(int i = 0; i < count && res; i++){
        res = AstTree_equals(List_at(self->children, i), List_at(other->children, i));
    }
    return res && AstNode_equals(self->value, other->value);
}
