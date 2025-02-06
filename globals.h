#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#ifndef YYPARSER

#include "cminus.tab.h"
#define ENDFILE 0

#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

/* MAXRESERVED = the number of reserved words */
#define MAXRESERVED 8
#define MAXTOKENLEN 40
#define INDENT indentno+=2
#define UNINDENT indentno-=2

extern int indentno;
extern char tokenString[MAXTOKENLEN+1];

typedef int TokenType;

extern FILE* source; /* source code text file */
extern FILE* listing; /* listing output text file */
extern FILE* code; /* code text file for TM simulator */

extern int lineno; /* source line number for listing */

/**************************************************/
/***********   Syntax tree for parsing ************/
/**************************************************/

typedef enum {StmtK,ExpK} NodeKind;
typedef enum {IfK,WhileK,AssignK,ReturnK,FunctionK} StmtKind;
typedef enum {OpK,ConstK,IdK,TypeK} ExpKind;

/* ExpType is used for type checking */
typedef enum {Void,Integer,Boolean} ExpType;

#define MAXCHILDREN 3

typedef struct treeNode
   { struct treeNode * child[MAXCHILDREN];
     struct treeNode * sibling;
     int lineno;
     NodeKind nodekind;
     union { StmtKind stmt; ExpKind exp;} kind;
     union { TokenType op;
             int val;
             char * name; } attr;
     ExpType type; /* for type checking of exps */
   } TreeNode;

void printToken(TokenType token, char * tokenString);

TreeNode * newStmtNode(StmtKind kind);

TreeNode * newExpNode(ExpKind kind);

char * copyString(char * s);

void printTree( TreeNode * tree );

TokenType getToken(void);

TreeNode * parse(void);

void printSpaces(void);

//void abrirArq(void);

extern int EchoSource;

extern int TraceScan;

extern int TraceParse;

extern int TraceAnalyze;

extern int TraceCode;

extern int Error; 

#endif
