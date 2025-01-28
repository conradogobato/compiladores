%{
#include <fstream>
#include <iostream>
using namespace std;

extern "C"
{
//    yyparse(void);
  int yylex(void);
  void abrirArq();
}

void yyerror(const char *);
%}

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
    | tipo_especificador ID ACO FCO
;

composto_decl:
    ACH local_declaracoes statement_lista FCH
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
    IF APR expressao FPR statement ELSE statement
    | IF APR expressao FPR statement
;

iteracao_decl:
    WHILE APR expressao FPR statement
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
    | ID ACO expressao FCO
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
    APR expressao FPR
    | var
    | ativacao
    | NUM
;

ativacao:
    ID APR args FPR
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

int main()
{
  cout << "\nParser em execução...\n";
  abrirArq();
  return yyparse();
}

void yyerror(const char * msg)
{
  extern char* yytext;
  cout << msg << ": " << yytext << endl;
}


