#include "../include/lexer.h"
#include "../include/list.h"
#include "../include/token.h"
#include "../include/string_buffer.h"
#include "../include/iterator.h"


#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static bool isNameChar(char chr){
    return isalnum(chr) || chr == '_';
}

int Lexer_splitTokens(char * input, List * tokens){
    size_t lineNum = 1;
    char ch = 0;
    int isUncorrect = 0;
    StringBuffer * sb = StringBuffer_new();
    while(1){
        ch = *input;
        if(ch == '\0'){
            break;
        }
        else if(isspace(ch)){
            while(isspace(*input)){
                if(*input == '\n'){
                    lineNum++;
                }
                input++;
            }
            continue;
        }else if(ch == '('){
            List_add(tokens, Token_new(TokenType_OPEN_BRACKETS, "", lineNum));
        }else if(ch == ')'){
            List_add(tokens, Token_new(TokenType_CLOSE_BRACKETS, "", lineNum));
        }else if(ch == '{'){
            List_add(tokens, Token_new(TokenType_OPEN_BLOCK, "", lineNum));
        }else if(ch == '}'){
            List_add(tokens, Token_new(TokenType_CLOSE_BLOCK, "", lineNum));
        }else if(ch == ';'){
            List_add(tokens, Token_new(TokenType_SEMICOLON, "", lineNum));
        }else if(ch == '+'){
            List_add(tokens, Token_new(TokenType_MATH_PLUS, "", lineNum));
        }else if(ch == '-'){
            List_add(tokens, Token_new(TokenType_MATH_MINUS, "", lineNum));
        }else if(ch == '*'){
            List_add(tokens, Token_new(TokenType_MATH_MULT, "", lineNum));
        }else if(ch == '/'){
            List_add(tokens, Token_new(TokenType_MATH_DIV, "", lineNum));
        }else if(ch == '%'){
            List_add(tokens, Token_new(TokenType_MATH_MOD, "", lineNum));
        }else if(ch == '='){
            if(*(input + 1) == '='){
                List_add(tokens, Token_new(TokenType_BOOL_EQUAL, "", lineNum));
                input++;
            }else{
                List_add(tokens, Token_new(TokenType_APPROPRIATION, "", lineNum));
            }
        }else if(ch == '!'){
            if(*(input + 1) == '='){
                List_add(tokens, Token_new(TokenType_BOOL_NOT_EQUAL, "", lineNum));
                input++;
            }else{
                List_add(tokens, Token_new(TokenType_BOOL_NOT, "", lineNum));
            }
        }else if(ch == '<'){
            if(*(input + 1) == '='){
                List_add(tokens, Token_new(TokenType_BOOL_LESS_EQUAL, "", lineNum));
                input++;
            }else{
                List_add(tokens, Token_new(TokenType_BOOL_LESS, "", lineNum));
            }
        }else if(ch == '>'){
            if(*(input + 1) == '='){
                List_add(tokens, Token_new(TokenType_BOOL_MORE_EQUAL, "", lineNum));
                input++;
            }else{
                List_add(tokens, Token_new(TokenType_BOOL_MORE, "", lineNum));
            }
        }else if(ch == '|'){
            if(*(input + 1) == '|'){
                List_add(tokens, Token_new(TokenType_BOOL_OR, "", lineNum));
                input++;
            }else{
                fprintf(stderr, "Error in line %lu, unknown operator '|'\n", lineNum);
                isUncorrect = 1;
                break;
            }
        }else if(ch == '&'){
            if(*(input + 1) == '&'){
                List_add(tokens, Token_new(TokenType_BOOL_AND, "", lineNum));
                input++;
            }else{
                fprintf(stderr, "Error in line %lu, unknown operator '&'\n", lineNum);
                isUncorrect = 1;
                break;
            }
        }else if(strncmp(input, "if", 2) == 0 && !isNameChar(*(input + 2))){
            List_add(tokens, Token_new(TokenType_IF, "", lineNum));
            input += 1;
        }else if(strncmp(input, "else", 4) == 0 && !isNameChar(*(input + 4))){
            List_add(tokens, Token_new(TokenType_ELSE, "", lineNum));
            input += 3;
        }else if(strncmp(input, "let", 3) == 0 && !isNameChar(*(input + 3))){
            if(!isspace(*(input + 3))){
                fprintf(stderr, "Error in line %lu, there is not space after \"let\"\n", lineNum);
                isUncorrect = 1;
                break;
            }
            List_add(tokens, Token_new(TokenType_LET, "", lineNum));
            input += 2;
        }else if(isdigit(ch)){
            int point = 0;
            while(isdigit(*input) || *input == '.'){
                if(*input == '.'){
                    point++;
                }
                StringBuffer_appendChar(sb, *input);
                input++;
            }
            if(point > 1){
                fprintf(stderr, "Error in line %lu, incorrect number\n", lineNum);
                isUncorrect = 1;
                break;
            }
            if(isalpha(*input)){
                fprintf(stderr, "Error in line %lu, incorrect number\n", lineNum);
                isUncorrect = 1;
                break;
            }
            List_add(tokens, Token_new(TokenType_NUMBER, StringBuffer_toNewString(sb), lineNum));
            StringBuffer_clear(sb);
            continue;
        }
        else if(isalpha(*input) || *input == '_'){
            while(isNameChar(*input)){
                StringBuffer_appendChar(sb, *input);
                input++;
            }
            
            List_add(tokens, Token_new(TokenType_ID, StringBuffer_toNewString(sb), lineNum));
            
            StringBuffer_clear(sb);
            continue;
        }
        else{
            fprintf(stderr, "Error in line %lu, incorrect symbol %c\n", lineNum, *input);
                isUncorrect = 1;
                break;
        }
        input++;

    }
    StringBuffer_free(sb);
    return isUncorrect;
}

void Lexer_clearTokens(List * tokens){
    size_t count = List_count(tokens);
    for(int i = 0; i < count ; i++){
        Token * token = (Token *)List_at(tokens, i);
        if(token->type == TokenType_NUMBER  || token->type == TokenType_ID){
            free((void *)token->name);
        }
        Token_free(token);
    }
    List_clear(tokens);
}

void Lexer_printTokensToFile(List * tokens, FILE * file){
    size_t count = List_count(tokens);
    size_t line = 0;
    for(size_t i = 0; i < count; i++){
        Token * token = (Token *)List_at(tokens, i);
        if(line != token->line){
            if(line != 0){
                fprintf(file, "\n");
            }
            line = token->line;
            fprintf(file, "%3lu|", token->line);
        }
        fprintf(file, "<");
        TokenType_printToFile(token->type, file);
        if(strlen(token->name)!= 0){
            fprintf(file, ",%s", token->name);
        }
        fprintf(file, "> ");
    }
    fprintf(file, "\n");
}