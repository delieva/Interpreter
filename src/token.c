#include "../include/token.h"
#include "../include/string_exp.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

Token * Token_new(TokenType type, const char * name, size_t line){
    Token * self = malloc(sizeof(Token));
    if(self == NULL){
        fprintf(stderr, "can not allocate memory\n");        
        abort();
    }
    self->name = name;
    self->type = type;
    self->line = line;
    return self;
}

void Token_free(Token * self){
    free(self);
}

bool Token_equals(Token * self, Token * other){
    if(self->type == other->type && strcmp(self->name, other->name) == 0){
        return true;
    }
    return false;
}


void TokenType_printToBuffer(TokenType type, char * buf, size_t bufLen){
    char string[bufLen];
    bufLen--;
    switch(type){
        case TokenType_MATH_PLUS:{
            strncpy(string, "+", bufLen);
            break;
        }
        case TokenType_MATH_MINUS:{
            strncpy(string, "-", bufLen);
            break;
        }
        case TokenType_MATH_MULT:{
            strncpy(string, "*", bufLen);
            break;
        }
        case TokenType_MATH_DIV:{
            strncpy(string, "/", bufLen);
            break;
        }
        case TokenType_MATH_MOD:{
            strncpy(string, "%", bufLen);
            break;
        }

        case TokenType_BOOL_AND:{
            strncpy(string, "&&", bufLen);
            break;
        }
        case TokenType_BOOL_OR:{
            strncpy(string, "||", bufLen);
            break;
        }
        case TokenType_BOOL_NOT:{
            strncpy(string, "!", bufLen);
            break;
        }
        case TokenType_BOOL_EQUAL:{
            strncpy(string, "==", bufLen);
            break;
        }
        case TokenType_BOOL_NOT_EQUAL:{
            strncpy(string, "!=", bufLen);
            break;
        }
        case TokenType_BOOL_LESS:{
            strncpy(string, "<", bufLen);
            break;
        }
        case TokenType_BOOL_MORE:{
            strncpy(string, ">", bufLen);
            break;
        }
        case TokenType_BOOL_LESS_EQUAL:{
            strncpy(string, "<=", bufLen);
            break;
        }
        case TokenType_BOOL_MORE_EQUAL:{
            strncpy(string, ">=", bufLen);
            break;
        }

        case TokenType_LET:{
            strncpy(string, "let", bufLen);
            break;
        }
        case TokenType_APPROPRIATION:{
            strncpy(string, "=", bufLen);
            break;
        }
        case TokenType_IF:{
            strncpy(string, "if", bufLen);
            break;
        }
        case TokenType_ELSE:{
            strncpy(string, "else", bufLen);
            break;
        }

        case TokenType_NUMBER:{
            strncpy(string, "NUM", bufLen);
            break;
        }

        case TokenType_ID:{
            strncpy(string, "ID", bufLen);
            break;
        }

        case TokenType_OPEN_BRACKETS:{
            strncpy(string, "(", bufLen);
            break;
        }
        case TokenType_CLOSE_BRACKETS:{
            strncpy(string, ")", bufLen);
            break;
        }
        case TokenType_OPEN_BLOCK:{
            strncpy(string, "{", bufLen);
            break;
        }
        case TokenType_CLOSE_BLOCK:{
            strncpy(string, "}", bufLen);
            break;
        }
        case TokenType_SEMICOLON:{
            strncpy(string, ";", bufLen);
            break;
        }
        default: {
            strncpy(string, "<?>", bufLen);
            break;
        }

    }
    snprintf(buf, bufLen, "%s", string);
}

void TokenType_printToFile(TokenType type, FILE * file){
    char str[62] = "";
    TokenType_printToBuffer(type, str, 32);
    fprintf(file, "%s", str);
}

char * Token_printToNewString(Token * token){
    switch((int)token->type){
        case TokenType_ID: 
        case TokenType_NUMBER:{
            return strDup(token->name);
        }
        default:{
            char * str = malloc(32 * sizeof(char));
            str[0] = '\'';
            TokenType_printToBuffer(token->type, str + 1, 30);
            int len = strlen(str);
            if(len < 31){
                str[len++] = '\'';
                str[len] = '\0';
            }
            return str;
        }
    }
}
