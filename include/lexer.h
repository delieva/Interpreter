#pragma once

#include "list.h"
#include <stdio.h>

int Lexer_splitTokens(char * input, List * tokens);
void Lexer_clearTokens(List * tokens);
void Lexer_printTokensToFile(List * tokens, FILE * file);