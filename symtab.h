#include "globals.h"

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

static int hash ( char * key );

void st_insert( char * name, int lineno, int loc, ExpType datatype, ExpKind idtype, char* scope );

int st_lookup ( char * name );

int st_lookup_type (char *name);

int st_lookup_scope (char *name, char * scope);

void printSymTab(FILE * listing);


#endif