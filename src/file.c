#include "../include/file.h"

#include <stdlib.h>
#include <stdio.h>


int fileExists(const char *fileName) {
    FILE *f = fopen(fileName, "rb");
    if (!f) return 0;  // false: not exists
    fclose(f);
    return 1;  // true: exists
}


long getFileSize(const char *fileName) {
    FILE *f = fopen(fileName, "rb");
    if (!f) return -1;  // error opening file
    fseek(f, 0, SEEK_END);  // rewind cursor to the end of file
    long fsize = ftell(f);  // get file size in bytes
    fclose(f);
    return fsize;
}

int readFileToBuffer(const char *fileName, char *buffer, int bufferLength) {
    FILE *f = fopen(fileName, "rb");
    if (!f) return 0;  // read 0 bytes from file
    long readBytes = fread(buffer, 1, bufferLength, f);
    buffer[readBytes] = '\0';
    readBytes++;
    fclose(f);
    return readBytes;  // number of bytes read
}