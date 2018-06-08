#pragma once

#include "tree.h"
#include "dict.h"

#include <stdio.h>

typedef enum {
    ValueType_ARRAY,
    ValueType_NUMBER,
    ValueType_BOOL,
    ValueType_UNDEFINED
} ValueType;

struct __Value {
   ValueType type;
   void * value;
};

typedef struct __Environment Environment;
struct __Environment {
    FILE * outputStream;
};


typedef struct __Program Program;
void Program_setError(Program * self, char * error);
FILE * Program_memstream(Program * self);

int Interpreter_execute(Tree * astTree, Environment * env);

typedef struct __Var Var;
typedef struct __Value Value;

Value * Value_newCopy(Value * origin);
Value * Value_newNumber(double number);
Value * Value_newBool(bool boolean);
Value * Value_newUndefined();
double Value_number(Value * self);
bool Value_bool(Value * self);
