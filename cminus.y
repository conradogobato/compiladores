%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int yylex();
void yyerror(const char *s);
%}

%token ELSE IF INT RETURN VOID WHILE
%token ID NUM
%token SOM SUB MUL DIV LT GT LET GET EQ DIF
%token ATR EOL VIR APR FPR ACO FCO ACH FCH
%token ERROR

%left SOM SUB
%left MUL DIV
%nonassoc LT GT LET GET EQ DIF

%%
programa:
    declaracao_lista
;

declaracao_lista:
    declaracao_lista declaracao
    | declaracao
;

declaracao:
    var_declaracao
    | fun_declaracao
;

var_declaracao:
    tipo_especificador ID EOL
    | tipo_especificador ID ACO NUM FCO EOL
;

tipo_especificador:
    INT
    | VOID
;

fun_declaracao:
    tipo_especificador ID APR params FPR composto_decl
;

params:
    param_lista
    | VOID
;

param_lista:
    param_lista VIR param
    | param
;

param:
    tipo_especificador ID
    | tipo_especificador ID '[' ']'
;

composto_decl:
    '{' local_declaracoes statement_lista '}'
;

local_declaracoes:
    local_declaracoes var_declaracao
    | /* vazio */
;

statement_lista:
    statement_lista statement
    | /* vazio */
;

statement:
    expressao_decl
    | composto_decl
    | selecao_decl
    | iteracao_decl
    | retorno_decl
;

expressao_decl:
    expressao EOL
    | EOL
;

selecao_decl:
    IF '(' expressao ')' statement ELSE statement
    | IF '(' expressao ')' statement
;

iteracao_decl:
    WHILE '(' expressao ')' statement
;

retorno_decl:
    RETURN EOL
    | RETURN expressao EOL
;

expressao:
    var ATR expressao
    | simples_expressao
;

var:
    ID
    | ID '[' expressao ']'
;

simples_expressao:
    soma_expressao relacional soma_expressao
    | soma_expressao
;

relacional:
    LT
    | GT
    | LET
    | GET
    | EQ
    | DIF
;

soma_expressao:
    soma_expressao SOM termo
    | soma_expressao SUB termo
    | termo
;

termo:
    termo MUL fator
    | termo DIV fator
    | fator
;

fator:
    '(' expressao ')'
    | var
    | ativacao
    | NUM
;

ativacao:
    ID '(' args ')'
;

args:
    arg_lista
    | /* vazio */
;

arg_lista:
    arg_lista VIR expressao
    | expressao
;

%%
void yyerror(const char *s) {
    fprintf(stderr, "Erro: %s\n", s);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo>\n", argv[0]);
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    yyin = inputFile;
    yyparse();

    fclose(inputFile);
    return 0;
}
