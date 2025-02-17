#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include <stdio.h>


static int location = 0;
static int first = 0;
static int mainDeclared = 0;

static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

void percorreArovre(TreeNode *t, char *scope){
  if(t == NULL) return;

  t->scope = scope;

  if(t->kind.exp == FunctionK) {
      
      percorreArovre(t->child[0], t->attr.name);
      percorreArovre(t->child[1], t->attr.name);
      percorreArovre(t->child[2], t->attr.name);
      percorreArovre(t->sibling, t->attr.name);
      return;
  }
  
  percorreArovre(t->child[0], scope);
  percorreArovre(t->child[1], scope);
  percorreArovre(t->child[2], scope);
  percorreArovre(t->sibling, scope);
}


static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}


static void insertNode(TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { case AssignK:
            if(st_lookup(t->attr.name) == -1){
                printf("Semantic error at row (%d): variable '%s' not declared\n", t->lineno, t->attr.name);
                exit(EXIT_FAILURE); 
            }
            else if(st_lookup_type(t->attr.name) == -1){
              printf("Semantic error at row (%d): variable '%s' type is not Integer\n", t->lineno, t->attr.name);
              exit(EXIT_FAILURE); 
            }
            // else{
            //     st_insert(t->attr.name,t->lineno,0,t->type, IdK, t->scope);
            // }
        default:
            break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { case IdK:
          if(st_lookup_scope(t->attr.name, t->scope) == -1){
            st_insert(t->attr.name,t->lineno,location++,t->type, t->kind.exp, t->scope);
          }
          else 
          st_insert(t->attr.name,t->lineno,0,t->type, t->kind.exp, t->scope);
          break;

        case CALLfunctionK:
            if ((st_lookup(t->attr.name) == -1) && strcmp(t->attr.name, "output") != 0 && strcmp(t->attr.name, "input") != 0 && strcmp(t->attr.name, "yield") != 0 && strcmp(t->attr.name, "sleep") != 0){
                printf("Semantic error at row (%d): function '%s' not declared\n", t->lineno, t->attr.name);
                exit(EXIT_FAILURE); 
            }
            else{
                st_insert(t->attr.name,t->lineno,0,t->type, t->kind.exp, t->scope);
            }
            break;

        case FunctionK:
            if (st_lookup(t->attr.name) == -1) {
                st_insert(t->attr.name, t->lineno, location++, t->type, t->kind.exp, t->scope);
                if (strcmp(t->attr.name, "main") == 0) {
                  mainDeclared = 1;
                }
            } else {
                printf("Semantic error at row (%d): function '%s' already declared\n", t->lineno, t->attr.name);
                exit(EXIT_FAILURE); 
            }
            break;

        case TypeK:
            if(t->child[0] == NULL) return;
            if((t->type == Void) && (t->child[0]->kind.exp != FunctionK)){
              printf("Semantic error at row (%d): variable '%s' declared void\n", t->lineno, t->child[0]->attr.name);
              exit(EXIT_FAILURE); 
            }
            break;

        default:
          break;
      }
      break;
    default:
      break;
  }
}

void buildSymtab(TreeNode * syntaxTree)
{ traverse(syntaxTree,insertNode,nullProc);
  if(mainDeclared == 0){
        printf("Semantic error: main function not declared\n");
        exit(EXIT_FAILURE);
  }
  if (TraceAnalyze)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}


static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { case ExpK:
      switch (t->kind.exp)
      { case OpK:
          if ((t->child[0]->type != Integer) ||
              (t->child[1]->type != Integer))
            typeError(t,"Op applied to non-integer");
          if ((t->attr.op == EQ) || (t->attr.op == LT))
            t->type = Boolean;
          else
            t->type = Integer;
          break;
        case ConstK:
        case IdK:
          t->type = Integer;
          break;
        default:
          break;
      }
      break;
    case StmtK:
      switch (t->kind.stmt)
      { case IfK:
          if (t->child[0]->type == Integer)
            typeError(t->child[0],"if test is not Boolean");
          break;
        case AssignK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"assignment of non-integer value");
          break;
        case WriteK:
          if (t->child[0]->type != Integer)
            typeError(t->child[0],"write of non-integer value");
          break;
        case ReturnK:
          if (t->child[1]->type == Integer)
            typeError(t->child[1],"repeat test is not Boolean");
          break;
        default:
          break;
      }
      break;
    default:
      break;

  }
}


void typeCheck(TreeNode * syntaxTree)
{ traverse(syntaxTree,nullProc,checkNode);
}
