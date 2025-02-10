#include "globals.h"

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

static int hash ( char * key );

void st_insert( char * name, int lineno, int loc, ExpType datatype, ExpKind idtype );

int st_lookup ( char * name );

void printSymTab(FILE * listing);


#endif