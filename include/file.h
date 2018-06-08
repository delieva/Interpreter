#pragma once


int fileExists(const char *fileName);
long getFileSize(const char *fileName);
int readFileToBuffer(const char *fileName, char *buffer, int bufferLength);