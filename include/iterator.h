#pragma once
#include <stdbool.h>

typedef struct __Iterator Iterator;
typedef long IteratorDistance;

void Iterator_free(Iterator * self);

void * Iterator_value    (Iterator * self);
void   Iterator_next     (Iterator * self);
void   Iterator_prev     (Iterator * self);
void   Iterator_advance  (Iterator * self, IteratorDistance n);
bool   Iterator_equals   (Iterator * self, Iterator * other);
IteratorDistance Iterator_distance (Iterator * begin, Iterator * end);