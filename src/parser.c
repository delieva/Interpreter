#include "../include/parser.h"
#include "../include/token.h"
#include "../include/iterator.h"
#include "../include/string_buffer.h"
#include "../include/tree.h"
#include "../include/ast.h"
#include "../include/string_exp.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define __SIZE(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))



typedef struct {
    Iterator * tokens;
    Iterator * tokensEnd;
    char * error;
    size_t level;
} Parser;

static bool eoi         (Parser * parser);
static void Parser_free (Parser * parser);
static Tree * prog      (Parser * parser);


Tree * Parser_buildNewAstTree(List * tokens){
    Parser parser = {
        .tokens = List_getNewBeginIterator(tokens),
        .tokensEnd = List_getNewEndIterator(tokens),
        .error = NULL,
        .level = -1
    };
    Tree * progTree = prog(&parser);
    if(parser.error){
        fprintf(stderr, ">>> ERROR: %s\n", parser.error);
        Tree * invalidProg = Tree_new(AstNode_new(AstNodeType_INVALID_PROGRAM, strDup("invalid program"), 0));
        List_add(invalidProg->children, progTree);
        Parser_free(&parser);
        return invalidProg;
    }else if(!eoi(&parser)){
        Token * token = Iterator_value(parser.tokens);
        char * tokenName = Token_printToNewString(token);
        fprintf(stderr, ">>> ERROR: Unexpected token <%s> in line [%lu] \n", tokenName, token->line);
        free(tokenName);
    }
    Parser_free(&parser);
    return progTree;
}




static bool eoi(Parser * parser){
    return Iterator_equals(parser->tokens, parser->tokensEnd);
}

static void Parser_free(Parser * parser){
    Iterator_free(parser->tokens);
    Iterator_free(parser->tokensEnd);
    if(parser->error) free(parser->error);
}

static Tree * accept(Parser * parser, TokenType tokenType, size_t line);

static Tree * var_arr_decl (Parser * parser);
static Tree * var_decl     (Parser * parser);
static Tree * st           (Parser * parser);
static Tree * st_expect    (Parser * parser);
static Tree * expr         (Parser * parser);
static Tree * expr_expect  (Parser * parser);
static Tree * expr_st      (Parser * parser);
static Tree * block_st     (Parser * parser);
static Tree * select_st    (Parser * parser);
static Tree * assign       (Parser * parser);
static Tree * assign_ap    (Parser * parser);
static Tree * log_or       (Parser * parser);
static Tree * log_or_ap    (Parser * parser);
static Tree * logic_and    (Parser * parser);
static Tree * logic_and_ap (Parser * parser);
static Tree * eq           (Parser * parser);
static Tree * eq_ap        (Parser * parser);
static Tree * rel          (Parser * parser);
static Tree * rel_ap       (Parser * parser);
static Tree * add          (Parser * parser);
static Tree * add_ap       (Parser * parser);
static Tree * mult         (Parser * parser);
static Tree * mult_ap      (Parser * parser);
static Tree * unary        (Parser * parser);
static Tree * primary      (Parser * parser);
static Tree * ID           (Parser * parser);
static Tree * NUMBER       (Parser * parser);
static Tree * var          (Parser * parser);
static Tree * parentheses  (Parser * parser);

typedef Tree * (*GrammarRule)(Parser * parser);

// EBNF: A = { B };
static bool ebnf_multiple      (Parser * parser,List * nodes,  GrammarRule rule);
// EBNF: A = B | C | D;
static Tree * ebnf_select        (Parser * parser, GrammarRule rules[], size_t rulesLen);
static Tree * ebnf_selectLexemes (Parser * parser, TokenType types[], size_t typesLen);

// EBNF: A = bA';
static Tree * ebnf_ap_main_rule(Parser * parser, GrammarRule next, GrammarRule ap);
// EBNF: A' = aA' | e;
static Tree * ebnf_ap_recursive_rule(
Parser * parser, 
TokenType types[], 
size_t typesLen, 
GrammarRule next, 
GrammarRule ap);


static AstNodeType TokenType_toAstNodeType(TokenType type){
    switch((int)type){
    case TokenType_MATH_PLUS: return AstNodeType_MATH_PLUS;
    case TokenType_MATH_MINUS: return AstNodeType_MATH_MINUS;
    case TokenType_MATH_MULT: return AstNodeType_MATH_MULT;
    case TokenType_MATH_DIV: return AstNodeType_MATH_DIV;
    case TokenType_MATH_MOD: return AstNodeType_MATH_MOD;

    case TokenType_BOOL_AND: return AstNodeType_BOOL_AND;
    case TokenType_BOOL_OR: return AstNodeType_BOOL_OR;
    case TokenType_BOOL_NOT: return AstNodeType_BOOL_NOT;
    case TokenType_BOOL_EQUAL: return AstNodeType_BOOL_EQUAL;
    case TokenType_BOOL_NOT_EQUAL: return AstNodeType_BOOL_NOT_EQUAL;
    case TokenType_BOOL_LESS: return AstNodeType_BOOL_LESS;
    case TokenType_BOOL_MORE: return AstNodeType_BOOL_MORE;
    case TokenType_BOOL_LESS_EQUAL: return AstNodeType_BOOL_LESS_EQUAL;
    case TokenType_BOOL_MORE_EQUAL: return AstNodeType_BOOL_MORE_EQUAL;

    case TokenType_APPROPRIATION: return AstNodeType_ASSIGN;
    case TokenType_LET: return AstNodeType_LET;
    case TokenType_IF: return AstNodeType_IF;
    case TokenType_NUMBER: return AstNodeType_NUMBER;
    case TokenType_ID: return AstNodeType_ID;
        default: return AstNodeType_UNKNOWN;
    }
}

static Tree * accept(Parser * parser, TokenType tokenType, size_t line){
    if(eoi(parser)) return NULL;
    Token * token = Iterator_value(parser->tokens);
    if(token->type == tokenType){
        AstNodeType astNode = TokenType_toAstNodeType(tokenType);
        char * astName = Token_printToNewString(token);
        AstNode * node = AstNode_new(astNode, astName, line);
        Tree * tree = Tree_new(node);
        Iterator_next(parser->tokens);
        return tree;
    }
    return NULL;
}

static bool accept_bool(Parser * parser, TokenType tokenType){
    if(eoi(parser)) return false;
    Token * token = Iterator_value(parser->tokens);
    if(token->type == tokenType){
        Iterator_next(parser->tokens);
        return true;
    }
    return false;
}

static size_t tokenLine(Parser * parser){
    size_t line = 0;
    if(eoi(parser)){
        if(Iterator_distance(parser->tokens, parser->tokensEnd) < 1){
            return 0;
        }
        Iterator_prev(parser->tokens);
        line = ((Token * )Iterator_value(parser->tokens))->line;
        Iterator_next(parser->tokens);
    }else{
        line = ((Token * )Iterator_value(parser->tokens))->line;
    }
    return line;
}


static void expect_err(Parser * parser, TokenType tokenType){
    if(parser->error) return;
    char * gotTokenType = "";
    bool isEoi = eoi(parser);
    int bufSize = 128;

    if(isEoi){
        gotTokenType = "end of input";
        Iterator_prev(parser->tokens);
    }else{
        char * buf = malloc(bufSize * sizeof(char));
        TokenType_printToBuffer(((Token *)Iterator_value(parser->tokens))->type, buf , bufSize);
        gotTokenType = buf;
    }
    char expectedTokenType[bufSize];
    TokenType_printToBuffer(tokenType, expectedTokenType , bufSize);

    StringBuffer * sb = StringBuffer_new();
    StringBuffer_appendFormat(sb, "expected <%s> got <%s> in line [%i]\n", 
        expectedTokenType, 
        gotTokenType, ((Token *)Iterator_value(parser->tokens))->line);
    parser->error = StringBuffer_toNewString(sb);

    if(!isEoi) {
        free(gotTokenType);
    }
    else {
        Iterator_next(parser->tokens);
    }
    StringBuffer_free(sb);
}

static bool expect_bool(Parser * parser, TokenType tokenType){
    bool res = accept_bool(parser, tokenType);
    if(res == true){
        return true;
    }
    expect_err(parser, tokenType);
    return false;
}


//************************************//

static void setErr_str(Parser * parser, const char * str){
    char gotToken[64];
    Token * token = Iterator_value(parser->tokens);
    TokenType_printToBuffer(token->type, gotToken, 64);
    StringBuffer * sb = StringBuffer_new();
    StringBuffer_appendFormat(sb, "Expected %s got <%s> in line [%i]", str, gotToken, token->line);
    parser->error = StringBuffer_toNewString(sb);
    StringBuffer_free(sb);
}

static Tree * prog        (Parser * parser){
    Tree * prorTree = Tree_new(AstNode_new(AstNodeType_PROGRAM, strDup("program"), 0));
    GrammarRule rules[] = {var_arr_decl, st};
    Tree * node = NULL;
    while((node = ebnf_select(parser, rules, __SIZE(rules))) && !parser->error){
        List_add(prorTree->children, node);
    }
    return prorTree;
}

static Tree * var_arr_decl    (Parser * parser){
    if(!accept_bool(parser, TokenType_LET)) return NULL;
    Tree * var_arr_declTree  = var_decl(parser);

    return var_arr_declTree;
}

static Tree * var_decl    (Parser * parser){
    Tree * idNode = ID(parser);
    if(idNode == NULL) return NULL;
    Tree * exprNode = NULL;
    if(accept_bool(parser, TokenType_APPROPRIATION)){
        exprNode = expr_expect(parser);
        if(parser->error){
            AstTree_free(idNode);
            return NULL;
        }
    }
    if(!expect_bool(parser, TokenType_SEMICOLON)){
        AstTree_free(idNode);
        if(exprNode)AstTree_free(exprNode);
        return NULL;
    }
    Tree * var_declTree = Tree_new(AstNode_new(AstNodeType_DECLARE_ID, strDup("declareVar"), tokenLine(parser)));
    List_add(var_declTree->children, idNode);
    if(exprNode)List_add(var_declTree->children, exprNode);
    return var_declTree;

}



static Tree * st (Parser * parser){
    GrammarRule rules[] = {expr_st, block_st, select_st};
    return ebnf_select(parser, rules, __SIZE(rules));
}

static Tree * st_expect    (Parser * parser){
    GrammarRule rules[] = {expr_st, block_st, select_st};
    Tree *  node = ebnf_select(parser, rules, __SIZE(rules));
    if(node == NULL && parser->error == NULL){
        if(!parser->error) setErr_str(parser, "<STATEMENT>");
    }
    return node;
}

static Tree * expr        (Parser * parser){
    return assign(parser);
}

static Tree * expr_expect  (Parser * parser){
    Tree * node = assign(parser);
    if(parser->error){
        if(node) AstTree_free(node);
        return NULL;
    }
    if(node == NULL){
        if(!parser->error) setErr_str(parser, "<EXPR>");
    }
    return node;
}

static Tree * expr_st     (Parser * parser){
    Tree * exprNode = expr(parser);
    if(exprNode){      
        if(expect_bool(parser, TokenType_SEMICOLON) == false){
            AstTree_free(exprNode);
            return NULL;
        }
    }else{
        accept_bool(parser, TokenType_SEMICOLON);
    }
    return exprNode;
}

static Tree * block_st    (Parser * parser){
    if(!accept_bool(parser, TokenType_OPEN_BLOCK)) return NULL;
    Tree * blockNode = Tree_new(AstNode_new(AstNodeType_BLOCK, strDup("block"), tokenLine(parser)));
    if(ebnf_multiple(parser,blockNode->children, st) == false){
        AstTree_free(blockNode);
        return NULL;
    }
    if(eoi(parser)){
        AstTree_free(blockNode);
        Iterator_prev(parser->tokens);
        if(!parser->error) setErr_str(parser, "<}> at the END of block");
        return NULL;
    }
    if(!expect_bool(parser, TokenType_CLOSE_BLOCK)){
        AstTree_free(blockNode);
    }
    return blockNode;
}

static Tree * select_st   (Parser * parser){
    if(accept_bool(parser, TokenType_ELSE)){
        Token * token = Iterator_value(parser->tokens);
        StringBuffer * sb = StringBuffer_new();
        StringBuffer_appendFormat(sb, "Else statement without IF in line %i", token->line);
        parser->error = StringBuffer_toNewString(sb);
        StringBuffer_free(sb);
        return NULL;
    }
    size_t line = tokenLine(parser);
    if(!accept_bool(parser, TokenType_IF) 
        || !expect_bool(parser, TokenType_OPEN_BRACKETS)) return NULL;
    Tree * testExprNode = expr_expect(parser);
    if(!testExprNode){
        return NULL;
    }
    if(!expect_bool(parser, TokenType_CLOSE_BRACKETS)){
        AstTree_free(testExprNode);
        return NULL;
    }
    Tree * stNode = st_expect(parser);
    if(!stNode){
        AstTree_free(testExprNode);
        return NULL;
    }
    Tree * ifNode = Tree_new(AstNode_new(AstNodeType_IF, strDup("if"), line));
    List_add(ifNode->children, testExprNode);
    List_add(ifNode->children, stNode);

    if(accept_bool(parser, TokenType_ELSE)){
        Tree * elseNode = st_expect(parser);
        if(elseNode == NULL){
            AstTree_free(ifNode);
            return NULL;
        }
        List_add(ifNode->children, elseNode);
    }
    return ifNode;
}


static Tree * assign      (Parser * parser){
    return ebnf_ap_main_rule(parser, log_or, assign_ap);
}

static Tree * assign_ap   (Parser * parser){
    TokenType types[] = {TokenType_APPROPRIATION};
    return ebnf_ap_recursive_rule(parser, types, __SIZE(types), log_or, assign_ap);
}

static Tree * log_or      (Parser * parser){
    return ebnf_ap_main_rule(parser, logic_and, log_or_ap);
}

static Tree * log_or_ap   (Parser * parser){
    TokenType types[] = {TokenType_BOOL_OR};
    return ebnf_ap_recursive_rule(parser, types, __SIZE(types), logic_and, log_or_ap);
}

static Tree * logic_and   (Parser * parser){
    return ebnf_ap_main_rule(parser, eq, logic_and_ap);
}

static Tree * logic_and_ap(Parser * parser){
    TokenType types[] = {TokenType_BOOL_AND};
    return ebnf_ap_recursive_rule(parser, types, __SIZE(types), eq, logic_and_ap);
}

static Tree * eq          (Parser * parser){
    return ebnf_ap_main_rule(parser, rel, eq_ap);
}

static Tree * eq_ap       (Parser * parser){
    TokenType types[] = {TokenType_BOOL_EQUAL, TokenType_BOOL_NOT_EQUAL};
    return ebnf_ap_recursive_rule(parser, types, __SIZE(types), rel, eq_ap);
}

static Tree * rel         (Parser * parser){
    return ebnf_ap_main_rule(parser, add, rel_ap);
}

static Tree * rel_ap      (Parser * parser){
    TokenType types[] = {TokenType_BOOL_MORE, TokenType_BOOL_MORE_EQUAL, 
        TokenType_BOOL_LESS, TokenType_BOOL_LESS_EQUAL};
    return ebnf_ap_recursive_rule(parser, types, __SIZE(types), add, rel_ap);
}


static Tree * add(Parser * parser) {
    return ebnf_ap_main_rule(parser, mult, add_ap);

}

static Tree * add_ap(Parser * parser) {
    TokenType types[] = {TokenType_MATH_PLUS, TokenType_MATH_MINUS};
    return ebnf_ap_recursive_rule(parser, types, __SIZE(types), mult, add_ap);
}

static Tree * mult(Parser * parser) {
    return ebnf_ap_main_rule(parser, unary, mult_ap);
}

static Tree * mult_ap(Parser * parser) {
    TokenType types[] = {TokenType_MATH_MULT, TokenType_MATH_DIV, TokenType_MATH_MOD};
    return ebnf_ap_recursive_rule(parser, types, __SIZE(types), unary, mult_ap);
}

static Tree * unary       (Parser * parser){
    TokenType types[] = {TokenType_BOOL_NOT, TokenType_MATH_MINUS, TokenType_MATH_PLUS};
    Tree * opNode = ebnf_selectLexemes(parser, types, __SIZE(types));
    Tree * primNode = primary(parser);
    if(primNode == NULL){
        if(opNode) AstTree_free(opNode);
        return NULL;
    }
    if(opNode){
        List_add(opNode->children, primNode);
        return opNode;
    }
    return primNode;
}

static Tree * primary     (Parser * parser){
    GrammarRule rules[] = {NUMBER, var, parentheses};
    return ebnf_select(parser, rules, __SIZE(rules));
}

static Tree * NUMBER      (Parser * parser){
    return accept(parser, TokenType_NUMBER, tokenLine(parser));
}
static Tree * ID          (Parser * parser){
    return accept(parser, TokenType_ID, tokenLine(parser));
}

static Tree * var (Parser * parser){
    Tree * varNode = ID(parser);
    if(varNode == NULL){
        return NULL;
    }
   
    if(parser->error){
        AstTree_free(varNode);
        return NULL;
    }
    return varNode;
}

static Tree * parentheses (Parser * parser){
    if(!accept_bool(parser, TokenType_OPEN_BRACKETS)) return NULL;
    Tree * exprNode = expr(parser);
    if(!expect_bool(parser, TokenType_CLOSE_BRACKETS)) {
        if(exprNode) AstTree_free(exprNode);
        return NULL;
    } 
    return exprNode;
}


//************************************//

static bool ebnf_multiple (Parser * parser, List * nodes,  GrammarRule rule){
    Tree * node = NULL;
    while((node = rule(parser)) && !parser->error){
        List_add(nodes, node);
    }
    return parser->error ? false : true;
}

static Tree * ebnf_select (Parser * parser, GrammarRule rules[], size_t rulesLen){
    Tree * node = NULL;
    for(int i = 0; i < rulesLen && !node; i++){
        GrammarRule rule = rules[i];
        node = rule(parser);
        if(parser->error) {
            return NULL;            
        }
    }
    return node;
}

static Tree * ebnf_selectLexemes (Parser * parser, TokenType types[], size_t typesLen){
    Tree * node = NULL;
    for(int i = 0; i < typesLen && !node; i++){
        node = accept(parser, types[i], tokenLine(parser));
    }
    return node;
}


static Tree * ebnf_ap_main_rule(Parser * parser, GrammarRule next, GrammarRule ap){
    Tree * nextNode = next(parser);
    if(nextNode){
        Tree * apNode = ap(parser);
        if(parser->error){
            AstTree_free(nextNode);
            return NULL;
        }
        if(apNode){
            List_insert(apNode->children, 0, nextNode);
            return apNode;
        }
        return nextNode;
    }
    return NULL;
}

static Tree * ebnf_ap_recursive_rule(Parser * parser, 
TokenType types[], 
size_t typesLen, 
GrammarRule next, 
GrammarRule ap){
    Tree * opNode = ebnf_selectLexemes(parser, types, typesLen);
    if(opNode == NULL) return NULL;
    Tree * node = NULL;
    Tree * nextNode = next(parser);
    if(nextNode == NULL){
        AstTree_free(opNode);
        if(!parser->error) setErr_str(parser, "<EXPR>");
        return NULL;
    }
    Tree * apNode = ap(parser);
    if(parser->error){
        AstTree_free(opNode);
        return NULL;
    }
    if(apNode){
        List_insert(apNode->children, 0, nextNode);
        node = apNode;
    }else{
        node = nextNode;
    }
    List_add(opNode->children, node);
    return opNode; 
}