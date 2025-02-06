%{
#include "globals.h"

char tokenString[MAXTOKENLEN+1];
extern int yylex(void);
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
{DIGITO}+          { return NUM; }

{LETRA}({LETRA}|{DIGITO})*   { return ID; }

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
        printf("Lexical Error: invalid character (non printable, ASCII code: %d) on row %d\n", yytext[0], lineno);
        exit(1);
    } else {
        printf("Lexical Error: invalid character '%s' on row: %d\n", yytext, lineno);
        exit(1);
    }
}

%%

TokenType getToken(void)
{ static int firstTime = TRUE;
  TokenType currentToken;
  if (firstTime)
  { firstTime = FALSE;
    lineno++;
    yyin = source;
    yyout = listing;
  }
  currentToken = yylex();
  strncpy(tokenString,yytext,MAXTOKENLEN);

  return currentToken;
}