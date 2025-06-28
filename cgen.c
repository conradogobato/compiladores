#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "cgen.h"
#include <stdio.h>

int emitLoc = 0;
Quad codecodecode[20];
int cont_reg = 0;
int temp_reg1 = 0;
int temp_reg2 = 0;

static BucketList *hashTable;
/*
void genExp(TreeNode *tree) {
    if (tree == NULL) return;

    switch (tree->kind.exp) {
        case ConstK:
            emit("LDC", 0, tree->attr.val, 0); // carrega constante em R0
            break;

        case IdK: {
            int loc = st_lookup_scope(tree->attr.name, tree->scope);
            emit("LD", 0, loc, 0); // carrega valor da variável
            break;
        }

        case OpK: {
            TreeNode *left = tree->child[0];
            TreeNode *right = tree->child[1];

            // gera código para o operando esquerdo
            genExp(left);
            emit("ST", 0, -1, 1); // salva R0 no topo da pilha (R1)

            // gera código para o operando direito
            genExp(right);
            emit("LD", 1, -1, 1); // carrega o operando esquerdo de volta

            switch (tree->attr.op) {
                case SOM:
                    emit("ADD", 0, 1, 0); // R0 = R1 + R0
                    break;
                case SUB:
                    emit("SUB", 0, 1, 0); // R0 = R1 - R0
                    break;
                case MUL:
                    emit("MUL", 0, 1, 0); // R0 = R1 * R0
                    break;
                case DIV:
                    emit("DIV", 0, 1, 0); // R0 = R1 / R0
                    break;
                default:
                    fprintf(stderr, "Erro: operador desconhecido.\n");
            }
            break;
        }

        case TypeK:
            break;

        // case CallK:
        //     // assume que os argumentos já foram empilhados
        //     for (int i = 0; i < tree->numChildren; i++) {
        //         genExp(tree->child[i]);
        //         emit("ST", 0, i, 1); // salva argumento
        //     }
        //     int addr = st_getFuncAddr(tree->attr.name);
        //     emit("CALL", 0, addr, 0); // chamada de função
        //     break;

        default:
            fprintf(stderr, "Erro: tipo de expressão não suportado.\n");
    }
}


void genStmt(TreeNode *tree) {
    if (tree == NULL) return;

    switch (tree->kind.stmt) {
        case AssignK: {
            genExp(tree->child[1]); // gera código para RHS
            int loc = st_lookup_scope(tree->attr.name, tree->scope);
            emit("ST", 0, loc, 0); // armazena resultado em memória
            break;
        }

        // case IfK: {
        //     genExp(tree->child[0]); // condição
        //     int jumpToElse = emitLoc++;
        //     genStmt(tree->child[1]); // then
        //     int jumpToEnd = emitLoc++;
        //     int elseLoc = emitLoc;
        //     genStmt(tree->child[2]); // else

        //     int endLoc = emitLoc;

        //     fprintf(code, "%3d: JEQ R0,0,%d\n", jumpToElse, elseLoc); // salto se falso
        //     fprintf(code, "%3d: JMP %d\n", jumpToEnd, endLoc);        // pular else
        //     break;
        // }

        // case WhileK: {
        //     int startLoc = emitLoc;
        //     genExp(tree->child[0]); // condição
        //     int jumpExit = emitLoc++;
        //     genStmt(tree->child[1]); // corpo
        //     emit("JMP", 0, startLoc, 0); // volta ao início
        //     fprintf(code, "%3d: JEQ R0,0,%d\n", jumpExit, emitLoc); // pula se falso
        //     break;
        // }

        // case ReturnK: {
        //     genExp(tree->child[0]); // gera valor de retorno
        //     emit("RET", 0, 0, 0);   // retorna
        //     break;
        // }

        // case CompoundK:
        //     for (int i = 0; i < MAXCHILDREN; i++) {
        //         genStmt(tree->child[i]);
        //     }
        //     break;

        default:
            fprintf(stderr, "Erro: tipo de comando não suportado.\n");
    }
}*/

void genExp(TreeNode *tree) {
    if (tree == NULL) return;

    char temp[20];

    switch (tree->kind.exp) {
        case ConstK: {
            sprintf(temp, "%d", tree->attr.val);
            emit("LOADI", temp, "", cont_reg++); // carrega constante em R0
            break;
        }

        case IdK: {
           // emit("LOAD", tree->attr.name, "", cont_reg++); // carrega valor da variável
            break;
        }

        case OpK: {
            TreeNode *left = tree->child[0];
            TreeNode *right = tree->child[1];

            // if(right->kind.exp == OpK){
            //     genExp(right);
            // }
            //genExp(left);
            temp_reg1 = cont_reg;
            emit("Load", left->attr.name, "", cont_reg++); // salva R0 no topo da pilha

            //emit("Store", left->attr.name, "", cont_reg++); // salva R0 no topo da pilha

            //genExp(right);
            temp_reg2 = cont_reg;
            emit("Load", right->attr.name, "", cont_reg++); // salva R0 no topo da pilha
            //emit("Store", right->attr.name, "", cont_reg++); // salva R0 no topo da pilha

            switch (tree->attr.op) {
                case SOM:
                    emit("ADD", get_reg(temp_reg1), get_reg(temp_reg2), cont_reg++); // R0 = R1 + R0
                    break;
                case SUB:
                    emit("SUB", get_reg(temp_reg1), get_reg(temp_reg2), cont_reg++); // R0 = R1 - R0
                    break;
                case MUL:
                    emit("MUL", get_reg(temp_reg1), get_reg(temp_reg2), cont_reg++); // R0 = R1 * R0
                    break;
                case DIV:
                    emit("DIV", get_reg(temp_reg1), get_reg(temp_reg2), cont_reg++); // R0 = R1 / R0
                    break;
                default:
                    fprintf(stderr, "Erro: operador desconhecido.\n");
            }
            break;
        }

        case TypeK:{
            if(tree->child[0]->kind.exp == FunctionK){
                emit("FUNC",tree->attr.name ,tree->child[0]->attr.name,0);
            }
            cGen(tree->child[0]);
            break;
        }

        case FunctionK:{
            cGen(tree->child[0]);
            cGen(tree->child[1]);
        }

        default:
            fprintf(stderr, "Erro: tipo de expressão não suportado.\n");
    }
}

void genStmt(TreeNode *tree) {
    if (tree == NULL) return;

    char temp[20];

    switch (tree->kind.stmt) {
        case AssignK: {
            cGen(tree->child[1]);
            int loc = st_lookup_scope(tree->attr.name, tree->scope);
            emit("STORE", tree->attr.name, "", --cont_reg); // guarda resultado na variável
            break;
        }
        case IfK:  {
            //if
            
        }
        default:
            fprintf(stderr, "Erro: tipo de comando não suportado.\n");
    }
}

void cGen(TreeNode *tree) {
    if (tree == NULL) return;

    switch (tree->nodekind) {
        case StmtK:
            genStmt(tree);
            break;
        case ExpK:
            genExp(tree);
            break;
    }

    cGen(tree->sibling);
}


// void cGen(TreeNode *tree) {
//     if (tree == NULL) return;
//     if (tree->nodekind == StmtK)
//         genStmt(tree);
//     else if (tree->nodekind == ExpK)
//         genExp(tree);

//     for (int i = 0; i < MAXCHILDREN; i++)
//         cGen(tree->child[i]);
//     cGen(tree->sibling);
// }

// void emit(char *op, int r, int d, int s) {
//     fprintf(codecodecode, "%3d: %s R%d,%d(R%d)\n", emitLoc++, op, r, d, s);
// }

void emit(char *op, char *arg1, char *arg2, int result) {
    strcpy(codecodecode[emitLoc].op, op);
    strcpy(codecodecode[emitLoc].arg1, arg1);
    strcpy(codecodecode[emitLoc].arg2, arg2);
    codecodecode[emitLoc].result = result;
    emitLoc++;
}

void printIntermediateCode() {
    for (int i = 0; i < emitLoc; ++i) {
        printf("%3d: (%s, %s, %s, R%d)\n", i,
               codecodecode[i].op,
               codecodecode[i].arg1,
               codecodecode[i].arg2,
               codecodecode[i].result);
    }
}

char* get_reg(int a) {
    char* temp = malloc(10 * sizeof(char));  // suficiente para "R" + número + '\0'
    if (temp == NULL) {
        fprintf(stderr, "Erro de alocação\n");
        exit(1);
    }
    sprintf(temp, "R%d", a);
    return temp;
}

// char* get_reg(int a){
//     char temp[10];
//     sprintf(temp, "R%d", a);

//     return temp;
// }