#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#define SIZE 211

#define SHIFT 4

static int hash ( char * key )
{ if (key == NULL) {
        fprintf(stderr, "Erro: chave NULL passada para hash()\n");
        return -1;  // Retorna um valor especial para indicar erro
  }
  
  int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the hash table */
static BucketList hashTable[SIZE];

void st_insert( char * name, int lineno, int loc, ExpType datatype, ExpKind idtype, char * scope) //int declare
{ int h = hash(name);
  BucketList l = hashTable[h];

  while ((l != NULL)){
    if(strcmp(name,l->name) == 0){
      if(strcmp(scope, l->scope) == 0){
        break;
      }
    }
    l = l->next;
  }

  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->datatype = datatype;
    l->idtype = idtype;
    l->scope = scope;
    l->lines->next = NULL;
    l->next = hashTable[h];
    hashTable[h] = l; }
  else /* found in table, so just add line number */
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} 

int st_lookup ( char * name )
{ int h = hash(name);
  BucketList l =  hashTable[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) return -1;
  else return l->memloc;
}

int st_lookup_type (char *name) {
    int h = hash(name);
    BucketList l = hashTable[h];
    while (l != NULL) {
        if (strcmp(name, l->name) == 0) {           
            if (l->idtype == 4) {
                return -1;
            } else {
                return 1;
            }
        }
        l = l->next;
    }
    return -1;
}

int st_lookup_void_func (char *name) {
  int h = hash(name);
  BucketList l = hashTable[h];
  while (l != NULL) {
      if (strcmp(name, l->name) == 0) {
        if(strcmp(name, "input") == 0 && strcmp(name, "output") == 0 && strcmp(name, "sleep") == 0) return 1;          
        if(strcmp(l->scope,"global") == 0 && l->datatype == Integer){
          return 1;
        }
      }
      l = l->next;
  }
  return -1;
}


int st_lookup_scope ( char * name, char * scope ) // novo no -> name: vet e scope: main e declare = 1
{ int h = hash(name);                         // tabela hash -> name: vet e scope: global
  BucketList l =  hashTable[h];

  while ((l != NULL)){
    if(strcmp(name,l->name) == 0){
      if(strcmp(scope, l->scope) == 0){
        return l->memloc;
      }
      if(strcmp(l->scope, "global") == 0){
        return -2;
      }
    }
    l = l->next;
  }
  return -1;
}


void printSymTab(FILE * listing)
{ int i;
  char * data;
  char * id;
  fprintf(listing,"Variable Name  Scope           Datatype  Idtype  Line Numbers\n");
  fprintf(listing,"-------------  -----           --------  ------  ------------\n");
  for (i=0;i<SIZE;++i)
  { if (hashTable[i] != NULL)
    { BucketList l = hashTable[i];
      while (l != NULL)
      { LineList t = l->lines;
        fprintf(listing,"%-14s ",l->name);
       // fprintf(listing,"%-8d  ",l->memloc);
        fprintf(listing,"%-14s  ",l->scope);
        switch (l->datatype)
        {
        case Integer:
          data = "int";
          break;
        case Void:
          data = "void";
          break;
        case Boolean:
          data = "boolean";
          break;
        default:
          data = "null";
          break;
        }
        fprintf(listing,"%-8s  ",data);
        switch (l->idtype)
        {
        case FunctionK:
          id = "func";
          break;
        case IdK:
          id = "var";
          break;
        case CALLfunctionK:
          id = "call func";
          break;
        default:
          break;
        }
        fprintf(listing,"%-8s  ",id);

        while (t != NULL)
        { fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
    }
  }
} 
