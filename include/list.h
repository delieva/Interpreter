#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "list.h"
#include "iterator.h"

typedef struct __List List;

List * List_new(void);
void List_free(List * self);

void List_add(List * self, void * value);
void List_insert(List * self, size_t index, void * value);
void * List_at(List * self, size_t index);
void List_set(List * self, size_t index, void * value);
bool List_removeAt(List * self, size_t index);
void List_clear(List * self);
size_t List_count(List * self);

Iterator * List_getNewBeginIterator (List * self);
Iterator * List_getNewEndIterator   (List * self);