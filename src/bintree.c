#include "../include/bintree.h"
#include <stdlib.h>

BinTree * BinTree_new(void * value) {
    BinTree * tree = malloc(sizeof(BinTree));
    tree->left = NULL;
    tree->right = NULL;
    tree->value = value;
    return tree;
}

void BinTree_free(BinTree * self) {
    free(self);
}
