#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "list.h"

typedef struct __Dict Dict;

Dict * Dict_new(void);
void Dict_free(Dict * self);

void   Dict_add      (Dict * self, char * key, void * value);
bool   Dict_contains (Dict * self, char * key);
void * Dict_get      (Dict * self, char * key);
void * Dict_set      (Dict * self, char * key, void * value);
void * Dict_remove   (Dict * self, char * key);
void   Dict_clear    (Dict * self);
size_t Dict_count    (Dict * self);

void   Dict_keys     (Dict * self, List * keys);
void   Dict_values   (Dict * self, List * values);