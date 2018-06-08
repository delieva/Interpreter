#pragma once

#include "../include/tree.h"

#include <stdbool.h>

typedef enum {
   AstNodeType_UNKNOWN,
   //
    AstNodeType_MATH_PLUS,
    AstNodeType_MATH_MINUS,
    AstNodeType_MATH_MULT,
    AstNodeType_MATH_DIV,
    AstNodeType_MATH_MOD,

    AstNodeType_BOOL_TRUE,
    AstNodeType_BOOL_FALSE,
    AstNodeType_BOOL_AND,
    AstNodeType_BOOL_OR,
    AstNodeType_BOOL_NOT,
    AstNodeType_BOOL_EQUAL,
    AstNodeType_BOOL_NOT_EQUAL,
    AstNodeType_BOOL_LESS,
    AstNodeType_BOOL_MORE,
    AstNodeType_BOOL_LESS_EQUAL,
    AstNodeType_BOOL_MORE_EQUAL,

    AstNodeType_ASSIGN,
    AstNodeType_LET,
    AstNodeType_IF,
    AstNodeType_WHILE,
    AstNodeType_NUMBER,
    AstNodeType_STRING,
    AstNodeType_ARRAY,
    AstNodeType_ID,
   //
    AstNodeType_ARGLIST,
    AstNodeType_BLOCK,
    AstNodeType_DECLARE_ID,
    AstNodeType_DECLARE_ARRAY,
    AstNodeType_PROGRAM,
    AstNodeType_INVALID_PROGRAM,
} AstNodeType;


typedef struct __AstNode AstNode;
struct __AstNode {
   AstNodeType type;
   char * name;
    size_t line;
};

AstNode * AstNode_new(AstNodeType type, char * name, size_t line);
void AstNode_free(AstNode * self);
void AstTree_free(Tree * self);
bool AstNode_equals(AstNode * self, AstNode * other);
bool AstTree_equals(Tree * self, Tree * other);
