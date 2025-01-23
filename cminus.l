%{
#include <stdlib.h>
#include <stdio.h>

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
#define SPACE 330
#define TAB 331
#define QDL 332
#define ERROR 333
FILE *outputFile;
int lineno = 0; 
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
{DIGITO}{DIGITO}*   return NUM;
{LETRA}{LETRA}*     return ID;
"/*"([^*]|(\*+[^*/]))*"*/" {
    for (char *c = yytext; *c != '\0'; ++c) {
        if (*c == '\n'){
            lineno++;
            return QDL;
        } 
    }
}
[ ]             return SPACE;
[\t]            return TAB;
[\n]            return QDL;
<<EOF>>         return FIM;
.               return ERROR;
%%

int yywrap() {
    return 1;
}

int main(int argc, char *argv[]) {
    FILE *inputFile = fopen(argv[1], "r");
    outputFile = fopen("output.txt", "w");

    yyin = inputFile;  
    int token;
    while ((token = yylex()) != FIM) {
        switch (token) {
            case SPACE:
                fprintf(outputFile, ""); 
                break;
            case TAB:
                fprintf(outputFile, "\t"); 
                break;
            case QDL:
                lineno++;
                fprintf(outputFile, "\n"); 
                break;
            case ELSE: 
                fprintf(outputFile, "ELSE "); 
                break;
            case IF: 
                fprintf(outputFile, "IF "); 
                break;
            case INT: 
                fprintf(outputFile, "INT "); 
                break;
            case RETURN: 
                fprintf(outputFile, "RETURN "); 
                break;
            case VOID: 
                fprintf(outputFile, "VOID "); 
                break;
            case WHILE: 
                fprintf(outputFile, "WHILE "); 
                break;
            case SOM: 
                fprintf(outputFile, "SOM "); 
                break;
            case SUB: 
                fprintf(outputFile, "SUB "); 
                break;
            case MUL: 
                fprintf(outputFile, "MUL "); 
                break;
            case DIV: 
                fprintf(outputFile, "DIV "); 
                break;
            case LT: 
                fprintf(outputFile, "LT "); 
                break;
            case GT: 
                fprintf(outputFile, "GT "); 
                break;
            case LET: 
                fprintf(outputFile, "LET "); 
                break;
            case GET: 
                fprintf(outputFile, "GET "); 
                break;
            case EQ: 
                fprintf(outputFile, "EQ "); 
                break;
            case DIF: 
                fprintf(outputFile, "DIF "); 
                break;
            case ATR: 
                fprintf(outputFile, "ATR "); 
                break;
            case EOL: 
                fprintf(outputFile, "EOL "); 
                break;
            case VIR: 
                fprintf(outputFile, "VIR "); 
                break;
            case APR: 
                fprintf(outputFile, "APR "); 
                break;
            case FPR: 
                fprintf(outputFile, "FPR "); 
                break;
            case ACO: 
                fprintf(outputFile, "ACO "); 
                break;
            case FCO: 
                fprintf(outputFile, "FCO "); 
                break;
            case ACH: 
                fprintf(outputFile, "ACH "); 
                break;
            case FCH: 
                fprintf(outputFile, "FCH "); 
                break;
            case NUM: 
                fprintf(outputFile, "NUM "); 
                break;
            case ID: 
                fprintf(outputFile, "ID "); 
                break;
            case ERROR:
                fprintf(stderr, "LEXICAL ERROR: %s at line %d\n", yytext, lineno); 
                fprintf(outputFile, "ERROR ");
                break;
        }
    }

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}