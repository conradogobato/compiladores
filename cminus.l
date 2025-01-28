%{
#include <stdlib.h>
#include <stdio.h>
#include "cminus.tab.h"
extern YYSTYPE yylval;
void abrirArq();

int lineno = 1; 
%}

DIGITO [0-9]
LETRA [a-zA-Z]

%%

else                return ELSE;
if                  return IF;
int                 return INT;
return              return RETURN;
void                return VOID;
while               return WHILE;
"+"                 return SOM;
"-"                 return SUB;
"*"                 return MUL;
"/"                 return DIV;
"<="                return LET;
">="                return GET;
"<"                 return LT;
">"                 return GT;
"=="                return EQ;
"!="                return DIF;
"="                 return ATR;
";"                 return EOL;
","                 return VIR;
"("                 return APR;
")"                 return FPR;
"["                 return ACO;
"]"                 return FCO;
"{"                 return ACH;
"}"                 return FCH;

{LETRA}({LETRA}|{DIGITO})*{DIGITO}({LETRA}|{DIGITO})*   return ERROR;
{DIGITO}+          { yylval = atoi(yytext); return NUM; }

{LETRA}({LETRA}|{DIGITO})*   { yylval = strdup(yytext); return ID; }

"/*"([^*]|(\*+[^*/]))*"*/" {
    for (char *c = yytext; *c != '\0'; ++c) {
        if (*c == '\n') lineno++;
    }
}

[ ]                 ; /* Ignore espaços */
[\t]                ; /* Ignore tabs */
\n              { lineno++; }
\r\n            { lineno++; }
<<EOF>>             return YYEOF;

.   { 
    if (yytext[0] < 32 || yytext[0] == 127) {
        printf("Erro: caractere inválido (não imprimível, código ASCII: %d)\n", yytext[0]);
    } else {
        printf("Erro: caractere inválido '%s'\n", yytext);
    }
}

%%

int yywrap() {
    return 1;
}

void abrirArq() {
    yyin = fopen("entrada.txt", "r");
    if (!yyin) {
        fprintf(stderr, "Erro ao abrir o arquivo entrada.txt\n");
        exit(EXIT_FAILURE);
    }
}