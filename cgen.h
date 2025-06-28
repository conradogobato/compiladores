#include "globals.h"
#include "symtab.h"

#ifndef _INTERCODE_H_
#define _INTERCODE_H_

typedef enum {opSum, OpSub, opMult, opDiv, opAnd, opXor, opJump, opJal, opJr, opBeq, opBne, opSlt, opSgt} opKind;

typedef enum {empty, constant, string} addrKind;

typedef struct {
    addrKind kind;
    int value;
} Address;

typedef struct {
    char op[10];
    char arg1[20];
    char arg2[20];
    int result;
} Quad;

extern Quad codecodecode[20];
extern int emitLoc;

void genExp(TreeNode *t);

void cGen(TreeNode *t);

void emit(char *op, char *arg1, char *arg2, int result);

void printIntermediateCode();

char* get_reg(int a);

#endif
