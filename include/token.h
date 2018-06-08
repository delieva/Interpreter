#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef enum {
    TokenType_MATH_PLUS,
    TokenType_MATH_MINUS,
    TokenType_MATH_MULT,
    TokenType_MATH_DIV,
    TokenType_MATH_MOD,


    TokenType_BOOL_AND,
    TokenType_BOOL_OR,
    TokenType_BOOL_NOT,
    TokenType_BOOL_EQUAL,
    TokenType_BOOL_NOT_EQUAL,
    TokenType_BOOL_LESS,
    TokenType_BOOL_MORE,
    TokenType_BOOL_LESS_EQUAL,
    TokenType_BOOL_MORE_EQUAL,

    TokenType_APPROPRIATION,
    TokenType_LET,
    TokenType_IF,
    TokenType_ELSE,
    TokenType_NUMBER,
    TokenType_ID,

    TokenType_OPEN_BRACKETS,
    TokenType_CLOSE_BRACKETS,
    TokenType_OPEN_BLOCK,
    TokenType_CLOSE_BLOCK,
    TokenType_SEMICOLON
} TokenType;

typedef struct __Token Token;
struct __Token {
   TokenType type;
   const char * name;
   size_t line;
};

Token * Token_new(TokenType type, const char * name, size_t line);
void Token_free(Token * self);

bool Token_equals(Token * self, Token * other);
char * Token_printToNewString(Token * token);
void TokenType_printToBuffer(TokenType type, char * buf, size_t bufLen);
void TokenType_printToFile(TokenType type, FILE * file);