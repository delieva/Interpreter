#include "../include/interpreter.h"
#include "../include/string_exp.h"
#include "../include/ast.h"
#include "../include/dict.h"
#include "../include/list.h"

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>


struct __Program{
    Dict * variables;
    Environment * env;
    char * error;
};

typedef Value * (*Function)(Program * program, List * values, size_t line);




void Program_setError(Program * self, char * error){
    self->error = error;
}

FILE * Program_memstream(Program * self){
    return self->env->outputStream;
}

static Value * eval(Program * program, Tree * node);


static Value * Value_new(ValueType type, void * value){
    Value * self = malloc(sizeof(Value));
    self->type = type;
    self->value = value;
    return self;
}

static void Value_free(Value * self){
    if(self && self->type != ValueType_ARRAY)free(self->value);
    free(self);
}



Value * Value_newNumber(double number){
    double * numberMem = malloc(sizeof(double));
    *numberMem = number;
    return Value_new(ValueType_NUMBER, numberMem);
}

Value * Value_newBool(bool boolean){
    bool * boolMem = malloc(sizeof(bool));
    *boolMem = boolean;
    return Value_new(ValueType_BOOL, boolMem);
}


Value * Value_newUndefined(){
    return Value_new(ValueType_UNDEFINED, NULL);
}

double Value_number(Value * self){
    assert(self->type == ValueType_NUMBER);
    return *((double *)self->value);
}

bool Value_bool(Value * self){
    assert(self->type == ValueType_BOOL);
    return *((bool *)self->value);
}

static bool Value_asBool(Value * self){
    switch((int)self->type){
        case ValueType_BOOL: return Value_bool(self);
        case ValueType_NUMBER: return fabs(Value_number(self)) > 1e-5;
        case ValueType_UNDEFINED: return false;
        default: assert(0 && "Not supported");
    }
}

static bool Value_equals(Value * a, Value * b){
    if(a->type != b->type) return false;
    switch(a->type) {
        case ValueType_BOOL: return Value_bool(a) == Value_bool(b); 
        case ValueType_NUMBER: return (fabs(Value_number(a) - Value_number(b))) < 1e-5;
        case ValueType_UNDEFINED: return true;
        default: assert(0 && "Not supported");
    }
}

static int Value_compare(Value * a, Value * b){
    if(a->type != b->type) return -1;
    if(Value_equals(a, b)) return 0;
    switch(a->type) {
        case ValueType_BOOL: {
            return Value_bool(a) > Value_bool(b) ? 1 : 2; 
        }
        case ValueType_NUMBER: {
            return Value_number(a) > Value_number(b) ? 1 : 2; 
        }
        default: assert(0 && "Not supported");
    }
}

Value * Program_getVariableValue(Program * self, char * key, size_t line){
    if(!Dict_contains(self->variables, key)){
        self->error = strDup_format("Use undeclared variables in line [%lu]", line);
        return NULL;
    }
    Value * val =  Dict_get(self->variables, key);
    return val;
}

Value * Value_newCopy(Value * origin){
    switch((int)origin->type){
        case ValueType_BOOL: return Value_newBool(Value_bool(origin));
        case ValueType_NUMBER: return Value_newNumber(Value_number(origin));
        case ValueType_UNDEFINED: return Value_newUndefined();
        default: assert(0 && "Not supported"); return NULL;
    }
}

static Value * eval(Program * program, Tree * node){
    AstNode * astNode = node->value;
    switch(astNode->type){
        case AstNodeType_ASSIGN:{
            Tree * firstNode = List_at(node->children, 0);
            AstNode * firstChild = firstNode->value;
            if(firstChild->type != AstNodeType_ID){
                program->error = strDup_format("Can`t assign to value in line [%lu]", astNode->line);
                return NULL;
            }
            char * varId = firstChild->name;
            Tree * secondNode = List_at(node->children, 1);
            Value * secondValue = eval(program, secondNode);
            if(program->error) return NULL;

            if(!Dict_contains(program->variables, varId)){
                program->error = strDup_format("Var for assign not found in line [%lu]", astNode->line);
                Value_free(secondValue);
                return NULL;
            }
            Value * oldVal =  Dict_set(program->variables, varId, Value_newCopy(secondValue));
            Value_free(oldVal);
            return secondValue;
        }
        case AstNodeType_ID:{
            char * varID = astNode->name;
            Value * varValue = Program_getVariableValue(program, varID, astNode->line);
            if(program->error){
                return NULL;
            }
            return Value_newCopy(varValue);
        }
        case AstNodeType_BOOL_TRUE:{
            return Value_newBool(true);
        }
        case AstNodeType_BOOL_FALSE:{
            return Value_newBool(false);
        }
        case AstNodeType_NUMBER: {
            double number = atof(astNode->name);
            return Value_newNumber(number);
        }
        case AstNodeType_MATH_MOD:{
            Tree * firstChild = List_at(node->children, 0);
            Value * firstValue = eval(program, firstChild);
            if(program->error) return NULL;
            
            Tree * secondChild = List_at(node->children, 1);
            Value * secondValue = eval(program, secondChild);
            if(program->error){
                Value_free(firstValue);
                return NULL;
            } 
            if(secondValue->type != ValueType_NUMBER){
                program->error = strDup_format("Invalid operation in line [%lu]", astNode->line);
                Value_free(firstValue);
                Value_free(secondValue);
                return NULL;
            }
            int val1 = (int)Value_number(firstValue);
            int val2 = (int)Value_number(secondValue);
            if(val1  == 0 || val2 == 0){
                program->error = strDup_format("Mod by zero in line [%lu]", astNode->line);
                Value_free(firstValue);
                Value_free(secondValue);
                return NULL;
            }
            int res = val1 % val2;
            Value_free(firstValue);
            Value_free(secondValue);
            return Value_newNumber(res);
        }
        case AstNodeType_MATH_PLUS:
        case AstNodeType_MATH_MINUS: {
            Tree * firstChild = List_at(node->children, 0);
            Value * firstValue = eval(program, firstChild);
            if(program->error) return NULL;

           

            if(firstValue->type != ValueType_NUMBER){
                program->error = strDup_format("Invalid operation in line [%lu]", astNode->line);
                Value_free(firstValue);
                return NULL;
            }            
            int nchild = List_count(node->children);
            if(nchild == 1){
                if(astNode->type == AstNodeType_MATH_MINUS) *(double *)firstValue->value *= -1;
                return firstValue;
            } else {
                Tree * secondChild = List_at(node->children, 1);
                Value * secondValue = eval(program, secondChild);
                if(program->error){
                    Value_free(firstValue);
                    return NULL;
                } 
                if(secondValue->type != ValueType_NUMBER){
                    program->error = strDup_format("Invalid operation in line [%lu]", astNode->line);
                    Value_free(firstValue);
                    Value_free(secondValue);
                    return NULL;
                }
                double res = astNode->type == AstNodeType_MATH_PLUS 
                    ? Value_number(firstValue) + Value_number(secondValue)
                    : Value_number(firstValue) - Value_number(secondValue);
                Value_free(firstValue);
                Value_free(secondValue);
                return Value_newNumber(res);
            }
        }
        case AstNodeType_MATH_MULT: {
            Tree * firstChild = List_at(node->children, 0);
            Value * firstValue = eval(program, firstChild);
            if(program->error) return NULL;
            if(firstValue->type != ValueType_NUMBER){
                program->error = strDup_format("Invalid operation in line [%lu]", astNode->line);
                Value_free(firstValue);
                return NULL;
            }
            
            Tree * secondChild = List_at(node->children, 1);
            Value * secondValue = eval(program, secondChild);
            if(program->error){
                Value_free(firstValue);
                return NULL;
            }
            if(secondValue->type != ValueType_NUMBER){
                program->error = strDup_format("Invalid operation in line [%lu]", astNode->line);
                Value_free(firstValue);
                Value_free(secondValue);
                return NULL;
            }
            double res = Value_number(firstValue) * Value_number(secondValue);
            Value_free(firstValue);
            Value_free(secondValue);
            return Value_newNumber(res);
        }
        case AstNodeType_MATH_DIV: {
            Tree * firstChild = List_at(node->children, 0);
            Value * firstValue = eval(program, firstChild);
            if(program->error) return NULL;
            if(firstValue->type != ValueType_NUMBER){
                program->error = strDup_format("Invalid operation in line [%lu]", astNode->line);
                Value_free(firstValue);
                return NULL;
            }
            
            Tree * secondChild = List_at(node->children, 1);
            Value * secondValue = eval(program, secondChild);
            if(program->error){
                Value_free(firstValue);
                return NULL;
            }
            if(secondValue->type != ValueType_NUMBER){
                program->error = strDup_format("Invalid operation in line [%lu]", astNode->line);
                Value_free(firstValue);
                Value_free(secondValue);
                return NULL;
            }
            if(fabs(Value_number(secondValue)) < 0.00001){
                program->error = strDup_format("Division by zero in line [%lu]", astNode->line);
                Value_free(firstValue);
                Value_free(secondValue);
                return NULL;
            }
            double res = Value_number(firstValue) / Value_number(secondValue);
            Value_free(firstValue);
            Value_free(secondValue);
            return Value_newNumber(res);
        }
        case AstNodeType_BOOL_NOT:{
            Tree * firstChild = List_at(node->children, 0);
            Value * firstValue = eval(program, firstChild);
            if(program->error) return NULL;

            bool res = !Value_asBool(firstValue);
            Value_free(firstValue);
            return Value_newBool(res);
        }
        case AstNodeType_BOOL_AND:
        case AstNodeType_BOOL_OR:
        case AstNodeType_BOOL_EQUAL:
        case AstNodeType_BOOL_NOT_EQUAL:
        case AstNodeType_BOOL_MORE:
        case AstNodeType_BOOL_MORE_EQUAL:
        case AstNodeType_BOOL_LESS:
        case AstNodeType_BOOL_LESS_EQUAL:{
            Tree * firstChild = List_at(node->children, 0);
            Value * firstValue = eval(program, firstChild);
            if(program->error) return NULL;
            
            
            Tree * secondChild = List_at(node->children, 1);
            Value * secondValue = eval(program, secondChild);
            if(program->error){
                Value_free(firstValue);
                return NULL;
            }
            bool res = false;
            switch((int)astNode->type){
                case AstNodeType_BOOL_AND:{
                    res = Value_asBool(firstValue) && Value_asBool(secondValue);
                    break;
                }
                case AstNodeType_BOOL_OR:{
                    res = Value_asBool(firstValue) || Value_asBool(secondValue);
                    break;
                }
                case AstNodeType_BOOL_EQUAL:{
                    res = Value_equals(firstValue, secondValue);
                    break;
                }
                case AstNodeType_BOOL_NOT_EQUAL:{
                    res = !Value_equals(firstValue, secondValue);
                    break;
                }
                case AstNodeType_BOOL_MORE:{
                    res = Value_compare(firstValue, secondValue) == 1 ? true : false;
                    break;
                }
                case AstNodeType_BOOL_MORE_EQUAL:{
                    res =  Value_compare(firstValue, secondValue) == 0 
                        || Value_compare(firstValue, secondValue) == 1 ? true : false;
                    break;
                }
                case AstNodeType_BOOL_LESS:{
                    res = Value_compare(firstValue, secondValue) == 2 ? true : false;
                    break;
                }
                case AstNodeType_BOOL_LESS_EQUAL:{
                    res =  Value_compare(firstValue, secondValue) == 0 
                        || Value_compare(firstValue, secondValue) == 2 ? true : false;
                    break;
                }
            }    
            Value_free(firstValue);
            Value_free(secondValue);
            return Value_newBool(res);
        }
        default: {
            assert(0 && "Not implicated");
        }
    }
    return NULL;
}




static void executeBlock(Program * program, Tree * blockTree);
static void executeIf(Program * program, Tree * ifTree);

static void executeStatement(Program * program, Tree * exTree){
    AstNode * value = exTree->value;
    switch((int)value->type){
        case AstNodeType_BLOCK:{
            executeBlock(program, exTree);
            break;
        }
        case AstNodeType_IF:{
            executeIf(program, exTree);
            break;
        }
        default:{
            Value * val = eval(program, exTree);
            if(program->error) break;
            Value_free(val);
            val = NULL;
        }
    }
}

static void executeBlock(Program * program, Tree * blockTree){
    AstNode * astNode = blockTree->value;
    assert(astNode->type == AstNodeType_BLOCK);
    for(int i = 0, count = List_count(blockTree->children); i < count; i++){
        Tree * childNode = List_at(blockTree->children, i);
        executeStatement(program, childNode);
        if(program->error) break;
    }
}



static void executeIf(Program * program, Tree * ifTree){
    AstNode * astNode = ifTree->value;
    assert(astNode->type == AstNodeType_IF);
    Tree * conditionNode = List_at(ifTree->children, 0);
    Value * condition = eval(program, conditionNode);
    if(program->error) return;
    bool conditionBool = Value_asBool(condition);
    Value_free(condition);
    if(conditionBool){
        executeStatement(program, List_at(ifTree->children, 1));
    }else if(List_count(ifTree->children) > 2){
        executeStatement(program, List_at(ifTree->children, 2));
    }
}


static void arrayFree(Value * arr){
    if(arr->type != ValueType_ARRAY) return;
    for(int i = 0, count = List_count(arr->value); i < count; i++){
        Value_free(List_at(arr->value, i));
    }
    List_free(arr->value);

}

static void Value_print(Value * self, FILE * streamToWrite){
    switch(self->type) {
        case ValueType_NUMBER: {
            double val = *(double *)self->value;
            fprintf(streamToWrite, "%.2lf", val);
            break;
        } 
        case ValueType_BOOL: {
            bool val = *(bool *)self->value;
            fprintf(streamToWrite, "%s", val ? "true" : "false");
            break;
        }
        case ValueType_UNDEFINED: {
            fprintf(streamToWrite, "undefined");
            break;
        }
        default: fprintf(streamToWrite, "<?>");
    }
}


int Interpreter_execute(Tree * astTree, Environment * env){
    AstNode * astNode = astTree->value;
    assert(astNode->type == AstNodeType_PROGRAM);
    Program program = {
        .variables = Dict_new(),
        .error = NULL,
        .env = env 
    };
    

    for(int i = 0, count = List_count(astTree->children); i < count; i++){
        Tree * childNode = List_at(astTree->children, i);
        AstNode * child = childNode->value;
        switch((int)child->type){
            case AstNodeType_DECLARE_ID:{
                Tree * fChildNode = List_at(childNode->children, 0);
                AstNode * fChild = fChildNode->value;
                Value * varValue = NULL;
                if(List_count(childNode->children) == 2){
                    Tree * sChildNode = List_at(childNode->children, 1);
                    varValue = eval(&program, sChildNode);
                }else{
                    varValue = Value_newUndefined();
                }

                char * varName = fChild->name;   
                if(program.error) {
                    Value_free(varValue); 
                    break;
                }
                if(Dict_contains(program.variables, varName)){
                    program.error = strDup_format("Duplicated variable id in line [%lu]", fChild->line);
                    Value_free(varValue);
                    break;
                }
                Dict_add(program.variables, varName, varValue);
                break;
            }
            case AstNodeType_BLOCK:{
                executeBlock(&program, childNode);
                break;
            }
            case AstNodeType_IF:{
                executeIf(&program, childNode);
                break;
            }
            default:{
                Value * val = eval(&program, childNode);
                Value_free(val);
                val = NULL;
            }
        }
        if(program.error) break;

        
    }

    if(!program.error){
        if(!Dict_contains(program.variables, "result")){
            Program_setError(&program, strDup("Variable \"result\" is not declared"));
        }
        else{
            fprintf(env->outputStream, "result: ");
            Value_print(Program_getVariableValue(&program, "result", 0), env->outputStream);
            fprintf(env->outputStream, "\n");
        }
    }
    List * vals = List_new();
    Dict_values(program.variables, vals);
    for(int i = 0, count = List_count(vals); i < count; i++){
        Value * val = List_at(vals, i);
        if(val->type == ValueType_ARRAY){
            arrayFree(val);
        }
        Value_free(val);
    }
    List_free(vals);

    Dict_free(program.variables);
    if(program.error){
        fprintf(program.env->outputStream , "Run-time error: %s\n", program.error);
        free(program.error);
        return 1;
    }

    return 0;
}