#include "globals.h"

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

typedef struct BucketListRec
   { char * name;
     LineList lines;
     int memloc ; /* memory location for variable */
     char * scope;
     int declare;
     ExpType datatype;
     ExpKind idtype;
     struct BucketListRec * next;
   } * BucketList;

static int hash ( char * key );

void st_insert( char * name, int lineno, int loc, ExpType datatype, ExpKind idtype, char* scope );

int st_lookup ( char * name );

int st_lookup_type (char *name);

int st_lookup_scope (char *name, char * scope);

void printSymTab(FILE * listing);


#endif