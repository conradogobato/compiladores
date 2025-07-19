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
    int line;
    char op[20];
    char arg1[20];
    char arg2[20];
    char arg3[20];
} Quad;

extern Quad codecodecode[100];
extern int emitLoc;

void genExp(TreeNode *t);

void codeGen(TreeNode *tree);

void cGen(TreeNode *t);

void emit(char *op, char *arg1, char *arg2, char *arg3);

void printIntermediateCode();

char* get_reg(int a);

char* newLabel();

char* int_to_string(int a);

void init_reg_stack();

int get_next_reg();

void free_reg(int reg);

#endif