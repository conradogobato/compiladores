%{
#include <fstream>
#include <stdio.h>
#include <iostream>
#include "globals.h"

#define YYPARSER
#define YYSTYPE TreeNode *
#define YYDEBUG 1

static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static TreeNode * savedTree; /* stores syntax tree for later return */

using namespace std;

extern int yydebug;

extern "C"
{
//    yyparse(void);
  int yylex(void);
  void abrirArq();
}
TreeNode * parse();
void yyerror(char *);
%}

%start programa
%token ELSE IF INT RETURN VOID WHILE
%token ID NUM
%token SOM SUB MUL DIV LT GT LET GET EQ DIF
%token ATR EOL VIR APR FPR ACO FCO ACH FCH
%token ERROR

%left SOM SUB
%left MUL DIV
%right LT GT LET GET EQ DIF

%%
programa:
    declaracao_lista    {
        savedTree = $1;
    }
;

declaracao_lista:
    declaracao_lista declaracao     {
        YYSTYPE t = $1;
            if (t != NULL){
            while (t->sibling != NULL)
                t = t->sibling;
            t->sibling = $2;
            $$ = $1;
            }
            else $$ = $2;
    }
    | declaracao    {
        $$ = $1;
    }
;

declaracao:
    var_declaracao  {
        $$ = $1;
    }
    | fun_declaracao    {
        $$ = $1;
    }
;

var_declaracao:
    tipo_especificador ID EOL   {
        $$ = $1;
    }
    | tipo_especificador ID ACO NUM FCO EOL {
        $$ = $1;
    }
;

tipo_especificador:
    INT {
        $$ = newExpNode(ConstK);
        $$->attr.val = atoi(tokenString);
        $$->type = Integer;
    }
    | VOID  {
        $$ = newExpNode(TypeK);
        $$->type = Void;
    }
;

fun_declaracao:
    tipo_especificador ID APR params FPR composto_decl  {
        $$ = newStmtNode(FunctionK);
        $$->child[0] = $1;
        $$->child[1] = $4;
        $$->child[2] = $6;
    }
;

params:
    param_lista {
        $$ = $1;
    }
    | VOID  {

    }
;

param_lista:
    param_lista VIR param     {
        YYSTYPE t = $1;
            if (t != NULL){
            while (t->sibling != NULL)
                t = t->sibling;
            t->sibling = $3;
            $$ = $1;
            }
            else $$ = $3;
    }
    | param {
        $$ = $1;
    }
;

param:
    tipo_especificador ID   {
        $$ = $1;
    }
    | tipo_especificador ID ACO FCO {
        $$ = $1;
    }
;

composto_decl:
    ACH local_declaracoes statement_lista FCH   {
        $$->child[0] = $2;
        $$->child[1] = $3;
    }
;

local_declaracoes:
    local_declaracoes var_declaracao  {
        YYSTYPE t = $1;
            if (t != NULL){
            while (t->sibling != NULL)
                t = t->sibling;
            t->sibling = $2;
            $$ = $1;
            }
            else $$ = $2;
    }
    | /* vazio */   {}
;

statement_lista:
    statement_lista statement   {
        YYSTYPE t = $1;
            if (t != NULL){
            while (t->sibling != NULL)
                t = t->sibling;
            t->sibling = $1;
            $$ = $1;
            }
            else $$ = $1;
    }
    | /* vazio */   {}
;

statement:
    expressao_decl  {
        $$ = $1;
    }
    | composto_decl  {
        $$ = $1;
    }
    | selecao_decl  {
        $$ = $1;
    }
    | iteracao_decl  {
        $$ = $1;
    }
    | retorno_decl  {
        $$ = $1;
    }
;

expressao_decl:
    expressao EOL   {
        $$ = $1;
    }
    | EOL   {}
;

selecao_decl:
    IF APR expressao FPR statement ELSE statement   {
        $$ = newStmtNode(IfK);
        $$->child[0] = $3;
        $$->child[1] = $5;
        $$->child[2] = $7;
    }
    | IF APR expressao FPR statement    {
        $$ = newStmtNode(IfK);
        $$->child[0] = $3;
        $$->child[1] = $5;
    }
;

iteracao_decl:
    WHILE APR expressao FPR statement   {
        $$ = newStmtNode(WhileK);
        $$->child[0] = $3;
        $$->child[1] = $5;
    }
;

retorno_decl:
    RETURN EOL  {
        $$ = newStmtNode(ReturnK);
    }
    | RETURN expressao EOL  {
        $$ = newStmtNode(ReturnK);
        $$->child[0] = $2;
    }
;

expressao:
    var ATR expressao   {
        $$ = newExpNode(OpK);
        $$->attr.op = ATR;
        $$->child[0] = $1;
        $$->child[1] = $3;
    }
    | simples_expressao {
        $$ = $1;
    }
;

var:
    ID  {
        $$ = newExpNode(IdK);
        $$->attr.name = copyString(tokenString);
    }
    | ID ACO expressao FCO  {
        $$ = newExpNode(IdK);
        $$->attr.name = copyString(tokenString);
        $$->child[0] = $3;
    }
;

simples_expressao:
    soma_expressao relacional soma_expressao    {
        $$ = $2;
        $$->child[0] = $1;
        $$->child[1] = $3;
    }
    | soma_expressao    {
        $$ = $1;
    }
;

relacional:
    LT  {
        $$ = newExpNode(OpK);
        $$->attr.op = LT;
    }
    | GT    {
        $$ = newExpNode(OpK);
        $$->attr.op = GT;
    }
    | LET   {
        $$ = newExpNode(OpK);
        $$->attr.op = LET;
    }
    | GET   {
        $$ = newExpNode(OpK);
        $$->attr.op = GET;
    }
    | EQ    {
        $$ = newExpNode(OpK);
        $$->attr.op = EQ;
    }
    | DIF   {
        $$ = newExpNode(OpK);
        $$->attr.op = DIF;
    }
;

soma_expressao:
    soma_expressao SOM termo    {
        $$ = newExpNode(OpK);
        $$->attr.op = SOM;
        $$->child[0] = $1;
        $$->child[1] = $3;
    }
    | soma_expressao SUB termo  {
        $$ = newExpNode(OpK);
        $$->attr.op = SUB;
        $$->child[0] = $1;
        $$->child[1] = $3;
    }
    | termo {
        $$ = $1;
    }
;

termo:
    termo MUL fator {
        $$ = newExpNode(OpK);
        $$->attr.op = MUL;
        $$->child[0] = $1;
        $$->child[1] = $3;
    }
    | termo DIV fator   {
        $$ = newExpNode(OpK);
        $$->attr.op = DIV;
        $$->child[0] = $1;
        $$->child[1] = $3;
    }
    | fator {
        $$ = $1;
    }
;

fator:
    APR expressao FPR   {
        $$ = $2;
    }
    | var   {
        $$ = $1;
    }
    | ativacao {
        $$ = $1;
    }
    | NUM   {
        $$ = newExpNode(ConstK);
        $$->attr.val = atoi(tokenString);
    }
;

ativacao:
    ID APR args FPR {
        $$ = newStmtNode(FunctionK);
        $$->attr.name = copyString(tokenString);
        $$ = $3;
    }
;

args:
    arg_lista   {
        $$ = $1;
    }
    | /* vazio */  {}
;

arg_lista:
    arg_lista VIR expressao  {
        YYSTYPE t = $1;
            if (t != NULL){
            while (t->sibling != NULL)
                t = t->sibling;
            t->sibling = $3;
            $$ = $1;
            }
            else $$ = $3;
    }
    | expressao {
        $$ = $1;
    }
;

%%

int main()
{
    printf("\nExecuting parser...\n");
    abrirArq();
    yydebug = 1;
    int parseResult = yyparse(); 
    if (parseResult == 0) {
        printf("\nSuccess!\n"); 
    } else {
        printf("\nError: parsing failed.\n"); 
    }
    return parseResult;
}

void yyerror(char *msg) {
    extern char *yytext;  
    extern int lineno;   
    printf("\nError: %s in row %d: Unexpected '%s'\n", msg, lineno, yytext);
}

TreeNode * parse(){
    yyparse();
    return savedTree;
}

static int yylex(){
    return getToken();
}