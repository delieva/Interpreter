#include "../include/string_exp.h"
#include "../include/string_buffer.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

char * strDup(const char *s) {
    char *d = malloc((strlen(s) + 1) * sizeof(char));   
    if(d == NULL){
        fprintf(stderr, "can not allocate memory\n");        
        abort();
    }          
    strcpy(d, s);                         
    return d;                         
}

char * strDup_format(const char * format, ...) {
    va_list argptr;
    va_start(argptr, format);
    size_t len = vsnprintf(NULL, 0, format, argptr) + 1;
    char buffer[len];
    va_start(argptr, format);
    vsnprintf(buffer, len, format, argptr);
    StringBuffer * sb =  StringBuffer_new();
    StringBuffer_append(sb, buffer);
    va_end(argptr);
    char * newStr = StringBuffer_toNewString(sb);
    StringBuffer_free(sb);
    return newStr;                         
}

char * strGetNew(void){
    size_t capasity = 16;
    char * str = malloc(capasity * sizeof(char));
    size_t pos = 0;
    char enterChar = getchar();
    while(enterChar != '\n'){
        if(pos >= capasity - 2){
            capasity *= 2;
            str = realloc(str, capasity * sizeof(char));
        }
        str[pos] = enterChar;
        pos++;
        enterChar = getchar();
    }
    str[pos] = '\0';
    return str;
}