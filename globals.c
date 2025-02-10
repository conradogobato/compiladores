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

void printSpaces(void)
{
  int i;
    for (i=0;i<indentno;i++)
      fprintf(listing," ");
}

void printTree( TreeNode * tree )
{ int i;
  INDENT;
  while (tree != NULL) {
    printSpaces();
    if (tree->nodekind==StmtK)
    { switch (tree->kind.stmt) {
        case IfK:
          fprintf(listing,"if:\n");
          break;
        case WhileK:
          fprintf(listing,"while:\n");
          break;
        case AssignK:
          fprintf(listing,"assign to: \n");
          break;
        case ReturnK:
          fprintf(listing,"return:\n");
          break;
        case WriteK:
          fprintf(listing,"start function:\n");
          break;
        default:
          fprintf(listing,"unknown ExpNode kind\n");
          break;
      }
    }
    else if (tree->nodekind==ExpK)
    { switch (tree->kind.exp) {
        case OpK:
          fprintf(listing,"op: ");
          printToken(tree->attr.op,"\0");
          break;
        case ConstK:
          fprintf(listing,"const: %d\n",tree->attr.val);
          break;
        case IdK:
          fprintf(listing,"id: %s\n",tree->attr.name);
          break;
        case TypeK:
          fprintf(listing,"type: %s\n",tree->attr.name);
          break;
        case FunctionK:
          fprintf(listing,"function: %s\n", tree->attr.name);
          break;
        case CALLfunctionK:
          fprintf(listing,"call to function: %s\n", tree->attr.name);
          break;
        default:
          fprintf(listing,"unknown ExpNode kind\n");
          break;
      }
    }
    else fprintf(listing,"unknown node kind\n");
    for (i=0;i<MAXCHILDREN;i++)
         printTree(tree->child[i]);
    tree = tree->sibling;
  }
  UNINDENT;
}


// void abrirArq(void) {
//     yyin = fopen("entrada.txt", "r");
//     if (!yyin) {
//         fprintf(stderr, "Erro ao abrir o arquivo entrada.txt\n");
//         exit(EXIT_FAILURE);
//     }
// }
