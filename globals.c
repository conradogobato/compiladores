#include "globals.h"

void printToken(TokenType token, char * tokenString) {
    switch (token) {
        case ELSE:   printf("ELSE "); break;
        case IF:     printf("IF "); break;
        case INT:    printf("INT "); break;
        case RETURN: printf("RETURN "); break;
        case VOID:   printf("VOID "); break;
        case WHILE:  printf("WHILE "); break;
        
        case ID:     printf("ID "); break;
        case NUM:    printf("NUM "); break;

        case SOM:    printf("+ "); break;
        case SUB:    printf("- "); break;
        case MUL:    printf("* "); break;
        case DIV:    printf("/ "); break;
        case LT:     printf("< "); break;
        case GT:     printf("> "); break;
        case LET:    printf("<= "); break;
        case GET:    printf(">= "); break;
        case EQ:     printf("== "); break;
        case DIF:    printf("!= "); break;
        
        case ATR:    printf("= "); break;
        case EOL:    printf("; "); break;
        case VIR:    printf(", "); break;
        case APR:    printf("( "); break;
        case FPR:    printf(") "); break;
        case ACO:    printf("[ "); break;
        case FCO:    printf("] "); break;
        case ACH:    printf("{ "); break;
        case FCH:    printf("} "); break;

        case ERROR:  printf("ERROR "); break;
        
        default:     printf("UNKNOWN TOKEN: %s ", tokenString); break;
    }
}

TreeNode * newStmtNode(StmtKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = StmtK;
    t->kind.stmt = kind;
    t->lineno = lineno;
  }
  return t;
}

TreeNode * newExpNode(ExpKind kind)
{ TreeNode * t = (TreeNode *) malloc(sizeof(TreeNode));
  int i;
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else {
    for (i=0;i<MAXCHILDREN;i++) t->child[i] = NULL;
    t->sibling = NULL;
    t->nodekind = ExpK;
    t->kind.exp = kind;
    t->lineno = lineno;
    t->type = Void;
  }
  return t;
}

char * copyString(char * s)
{ int n;
  char * t;
  if (s==NULL) return NULL;
  n = strlen(s)+1;
  t = (char *)malloc(n);
  if (t==NULL)
    fprintf(listing,"Out of memory error at line %d\n",lineno);
  else strcpy(t,s);
  return t;
}