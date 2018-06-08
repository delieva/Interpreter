#pragma once

typedef struct __StringBuffer StringBuffer;

StringBuffer * StringBuffer_new(void);
void StringBuffer_free(StringBuffer * self);

void StringBuffer_append(StringBuffer * self, const char * str);
void StringBuffer_appendChar(StringBuffer * self, char ch);
void StringBuffer_appendFormat(StringBuffer * self, const char * fmt, ...);
void StringBuffer_clear(StringBuffer * self);
char * StringBuffer_toNewString(StringBuffer * self);