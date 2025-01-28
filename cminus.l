%{
#include <stdlib.h>
#include <stdio.h>
void abrirArq();

#define ELSE 300
#define IF 301
#define INT 302
#define RETURN 303
#define VOID 304
#define WHILE 305
#define SOM 306
#define SUB 307
#define MUL 308
#define DIV 309
#define LT 310
#define GT 311
#define LET 312
#define GET 313
#define EQ 314
#define DIF 315
#define ATR 316
#define EOL 317
#define VIR 318
#define APR 319
#define FPR 320
#define ACO 321
#define FCO 322
#define ACH 323
#define FCH 324
#define NUM 328
#define ID 327
#define FIM 329
#define QDL 332
#define ERROR 333

FILE *outputFile;
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
{DIGITO}+           return NUM;
{LETRA}({LETRA}|{DIGITO})*   return ID;

"/*"([^*]|(\*+[^*/]))*"*/" {
    for (char *c = yytext; *c != '\0'; ++c) {
        if (*c == '\n') lineno++;
    }
}

[ ]                 ; /* Ignore espaços */
[\t]                ; /* Ignore tabs */
[\n]                { lineno++; }
<<EOF>>             return FIM;

.                   { printf("Erro: caractere inválido '%s'\n", yytext); }

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
