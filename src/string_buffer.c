#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "../include/string_buffer.h"
#include "../include/string_exp.h"


const int INITIAL_CAPACITY = 16;

typedef struct __StringBuffer {
    char * buffer;
    size_t capacity;
    size_t length;

} StringBuffer;

StringBuffer * StringBuffer_new(void){
    StringBuffer * self = malloc(sizeof(StringBuffer));
    if(self == NULL){
        fprintf(stderr, "can not allocate memory\n");        
        abort();
    }
    self->capacity = INITIAL_CAPACITY;
    self->buffer = malloc(sizeof(char) * self->capacity);
    self->buffer[0] = '\0';
    self->length = 1;
    return self; 
}

void StringBuffer_free(StringBuffer * self){
    free(self->buffer);
    free(self);
}

void StringBuffer_append(StringBuffer * self, const char * str){
    size_t len = strlen(str);
    if(self->length + len > self->capacity){
        size_t newCapacity = self->capacity * 2;
        while(newCapacity < self->length + len){
            newCapacity *= 2;
        }
        char * newBuffer = realloc(self->buffer, newCapacity * sizeof(char));
        if(newBuffer == NULL){
            fprintf(stderr, "can not allocate memory\n");        
            abort();
        }
        else {
            self->buffer = newBuffer;
            self->capacity = newCapacity;
        }
    }
    strcat(self->buffer + (self->length - 1), str);
    self->length += len;
}

void StringBuffer_appendChar(StringBuffer * self, char ch){
    if(self->capacity == self->length){
        self->buffer = realloc(self->buffer, self->capacity * 2);
        if(self->buffer == NULL){
            fprintf(stderr, "can not allocate memory\n");        
            abort();
        }
        self->capacity *= 2;
    }
    self->buffer[self->length] = '\0';
    self->buffer[self->length - 1] = ch;
    self->length++;
}

void StringBuffer_appendFormat(StringBuffer * self, const char * format, ...){
    va_list argptr;
    va_start(argptr, format);
    size_t len = vsnprintf(NULL, 0, format, argptr) + 1;
    char buffer[len];
    va_start(argptr, format);
    vsnprintf(buffer, len, format, argptr);
    StringBuffer_append(self, buffer);
    va_end(argptr);
}

void StringBuffer_clear(StringBuffer * self){
    self->buffer[0] = '\0';
    self->length = 1;
    self->capacity = INITIAL_CAPACITY;
    self->buffer = realloc(self->buffer, self->capacity);
    if(self->buffer == NULL){
        fprintf(stderr, "can not allocate memory\n");        
        abort();
    }
}


char * StringBuffer_toNewString(StringBuffer * self){
    return strDup(self->buffer);
}