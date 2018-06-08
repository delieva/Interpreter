#include "include/lexer.h"
#include "include/list.h"
#include "include/token.h"
#include "include/string_buffer.h"
#include "include/file.h"
#include "include/parser.h"
#include "include/tree.h"
#include "include/tree_printer.h"
#include "include/ast.h"
#include "include/interpreter.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


int main(int argc, char * argv[]){
    if(argc != 2){
        fprintf(stderr, "invalid argument");
    }
    const char * fileName = argv[1];
    if(!fileExists(fileName)){
        fprintf(stderr, "file is not exist");
        return EXIT_FAILURE;
    }
    size_t size = getFileSize(fileName);
    char * buffer = malloc(sizeof(char) * (size + 1));

    readFileToBuffer(fileName, buffer, size + 1);
    List * tokens = List_new();
    if(Lexer_splitTokens(buffer, tokens) == 1){
        free(buffer);
        Lexer_clearTokens(tokens);
        List_free(tokens);
        return EXIT_FAILURE;
    }
    free(buffer);


    Tree * root = Parser_buildNewAstTree(tokens);
    Lexer_clearTokens(tokens);
    List_free(tokens);
    if(root == NULL || ((AstNode *)root->value)->type != AstNodeType_PROGRAM){
        AstTree_free(root);
        return EXIT_FAILURE;
    }
    const char * fileTreeName = "ast_tree.txt";
    FILE * fileTree = fopen(fileTreeName, "w");
    if(fileTree){
        AstTree_printToFile(root, fileTree);
    }
    
    Environment env = { .outputStream = stdout };
    Interpreter_execute(root, &env);
    AstTree_free(root);
    return EXIT_SUCCESS;
}
