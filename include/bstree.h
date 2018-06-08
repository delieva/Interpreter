#pragma once

#include <stdbool.h>
#include <stdlib.h>

#include "list.h"

typedef struct __BSTree BSTree;


BSTree * BSTree_new(void);
void BSTree_free(BSTree * self);

void   BSTree_insert   (BSTree * self, int key, void * value, char * keyName);
bool   BSTree_lookup   (BSTree * self, int key);
void * BSTree_search   (BSTree * self, int key);
void * BSTree_delete   (BSTree * self, int key);
void * BSTree_set(BSTree * self, int key, void * value);

void   BSTree_clear    (BSTree * self);
size_t BSTree_count    (BSTree * self);

void   BSTree_keys     (BSTree * self, List * keys);
void   BSTree_values   (BSTree * self, List * values);
