


#include "globals.h"
#include "symtab.h"
#include "analyze.h"
#include "cgen.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_REGS 100

int emitLoc = 0;
Quad codecodecode[100];
int cont_reg = 0;

int labelCount = 0;

static BucketList *hashTable;


int reg_stack[MAX_REGS];
int reg_stack_top = 100;

void init_reg_stack() {
    reg_stack_top = 0;  // Reinicia o topo da pilha
    for (int i = 0; i < MAX_REGS; i++) {
        reg_stack[i] = i;  // Inicializa com todos os registradores disponíveis
    }
    reg_stack_top = MAX_REGS;  // Todos os registradores estão disponíveis inicialmente
}

int get_next_reg() {
    if (reg_stack_top <= 0) {
        fprintf(stderr, "Erro: sem registradores disponíveis\n");
        exit(1);
    }
    return reg_stack[--reg_stack_top];  // Pega o próximo registrador disponível
}

void free_reg(int reg) {
    if (reg_stack_top >= MAX_REGS) {
        fprintf(stderr, "Erro: pilha de registradores cheia\n");
        return;
    }
    reg_stack[reg_stack_top++] = reg;  // Libera o registrador
}

void genExp(TreeNode *tree) {
    //fprintf(stderr,"genExp talvez nulo\n");
    if (tree == NULL) return;
    //fprintf(stderr,"genExp nao nulo\n");

    char temp[20];

    switch (tree->kind.exp) {
        case ConstK: {
            //temp_reg1 = get_next_reg();
            fprintf(stderr,"constK: %d\n", tree->attr.val);
            sprintf(temp, "%d", tree->attr.val);
            emit("LOADI", temp, "", get_reg(cont_reg)); // carrega constante em R0
            tree->check = 1;
            //tree->reg_value = temp_reg1;
            break;
        }

        case IdK: {
            //temp_reg1 = get_next_reg();
            fprintf(stderr,"idK: %s\n", tree->attr.name);
            if(tree->param == 1) emit("LOAD", tree->attr.name, "", get_reg(cont_reg)); // carrega valor da variável
            //tree->reg_value = temp_reg1;
            break;
        }

        case OpK: {
            TreeNode *left = tree->child[0];
            TreeNode *right = tree->child[1];
            fprintf(stderr,"opK right: %d\n",right->kind.exp);

            int result_reg, temp_reg1 = 0, temp_reg2 = 0;

            if((left->kind.exp == ConstK) || (left->kind.exp == OpK)){
                cGen(left);
                temp_reg1 = cont_reg++;
                tree->check = 1;     
            }else {
                temp_reg1 = cont_reg; 
                emit("LOAD", left->attr.name, "-", get_reg(cont_reg++));
                
            }// salva R0 no topo da pilha

            fprintf(stderr,"Passou cGen right %d \n", right->kind.exp);            

            if((right->kind.exp == ConstK) || (right->kind.exp == OpK)){
                cGen(right);
                temp_reg2 = cont_reg++;
                tree->check = 1;
                
            }else {
                temp_reg2 = cont_reg; 
                emit("LOAD", right->attr.name, "-", get_reg(cont_reg++));
                
            }

            fprintf(stderr,"Passou cGen right \n");
            switch (tree->attr.op) {
                case SOM:
                    emit("ADD", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg)); // R0 = R1 + R0
                    break;
                case SUB:
                    emit("SUB", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg)); // R0 = R1 - R0
                    break;
                case MUL:
                    emit("MUL", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg)); // R0 = R1 * R0
                    break;
                case DIV:
                    emit("DIV", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg));  // R0 = R1 / R0
                    break;
                case LT: // Less Than (<)
                    emit("LT", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg));
                    break;
                case GT: // Greater Than (>)
                    emit("GT", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg));
                    break;
                case LET: // Less or Equal Than (<=)
                    emit("LE", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg));
                    break;
                case GET: // Greater or Equal Than (>=)
                    emit("GE", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg));
                    break;
                case EQ: // Equal (==)
                    emit("EQ", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg));
                    break;
                case DIF: // Different (!=)
                    emit("NE", get_reg(temp_reg1), get_reg(temp_reg2), get_reg(cont_reg));
                    break;
                default:
                    fprintf(stderr, "Erro: operador desconhecido.\n");
            }

            // Libera registradores temporários (se não vierem de subexpressões)
            if(!(left->kind.exp == ConstK || left->kind.exp == OpK)) {
                free_reg(temp_reg1);
            }
            if(!(right->kind.exp == ConstK || right->kind.exp == OpK)) {
                free_reg(temp_reg2);
            }

            // Armazena o registrador do resultado no nó atual
            tree->reg_value = result_reg;

            break;
        }

        case TypeK:{
            fprintf(stderr,"typeK\n");
            if(tree->child[0] == NULL) return;
            
            if(tree->child[0]->kind.exp == FunctionK){
                fprintf(stderr,"typeK son FunctionK: %s\n",tree->child[0]->attr.name);
                emit("FUNC",tree->child[0]->attr.name, "- ","-");
                tree->check = 1;
                cGen(tree->child[0]);
                emit("END FUNCTION",tree->child[0]->attr.name, "- ","-");
                //if(strcmp(tree->child[0]->attr.name, 'main') == 0) emit("HALT","-", "- ","-");
            }
            break;
        }

        case FunctionK:{
            fprintf(stderr,"functionK\n");
            return;
        }

        case CALLfunctionK: {
            fprintf(stderr, "Gerando código para chamada de função: %s\n", tree->attr.name);
            tree->check = 1; 
            
            TreeNode *arg = tree->child[0];
            int arg_count = 0;
            char *arg_regs[10];
            fprintf(stderr, "Gerando código para chamada de função: %s\n", tree->attr.name);

            while (arg != NULL) {
                fprintf(stderr, " de função: inside while\n");
                arg->param = 1;
                cGen(arg); 

                emit("PARAM",  get_reg(cont_reg++), "", "");
                arg_count++;
                arg = arg->sibling;
                fprintf(stderr, " de função:\n");
            }
            
            emit("CALL", tree->attr.name, int_to_string(arg_count), get_reg(cont_reg));
            
            fprintf(stderr, " de função pos emit:\n");
            return; 
        }
        default:
            fprintf(stderr, "Erro Exp: %s\n%d\n", tree->attr.name, tree->kind.exp);
    }
}

void genStmt(TreeNode *tree) {
    fprintf(stderr,"genStmt talvez nulo\n");
    if (tree == NULL) return;
    fprintf(stderr,"genStmt nao nulo\n");

    char temp[20];

    switch (tree->kind.stmt) {
        case AssignK: {
            fprintf(stderr,"assignK: %s --->  %d\n", tree->child[0]->attr.name, tree->child[1]->attr.name);
            cGen(tree->child[1]);
            tree->check = 1;
            int loc = st_lookup_scope(tree->attr.name, tree->scope);
            emit("STORE", tree->attr.name, "", get_reg(cont_reg++)); 
            break;
        }
        case IfK:  {
            
            TreeNode *p1 = tree->child[0]; 
            TreeNode *p2 = tree->child[1]; 
            TreeNode *p3 = tree->child[2]; 
            fprintf(stderr,"ifK: %d\n", p1->attr.name);
            tree->check = 1;
            
            cGen(p1);
            
            char *label1 = newLabel(); 
            emit("JUMP_FALSE", get_reg(cont_reg), "_", label1);

            cGen(p2);
            
            if (p3 != NULL) {
                fprintf(stderr,"ifK entrou p3:\n");
                char *label2 = newLabel(); 
                emit("JUMP", "_", "_", label2);
                emit("LABEL", "_", "_", label1);
                cGen(p3);
                emit("LABEL", "_", "_", label2);
            } else {
                emit("LABEL", "_", "_", label1);
            }
            break;
        }
        case WhileK:{
            TreeNode *p1 = tree->child[0]; 
            TreeNode *p2 = tree->child[1]; 

            char *label1 = newLabel(); 
            char *label2 = newLabel(); 

            emit("LABEL", "_", "_", label1);
            tree->check = 1;

            cGen(p1);
            
            emit("JUMP_FALSE", get_reg(cont_reg), "- ", label2);

            cGen(p2);
            
            emit("JUMP", "_", "_", label1);

            emit("LABEL", "_", "_", label2);
            break;
        }
        case WriteK:{
            fprintf(stderr,"WriteK\n");
            return;}

        case ReturnK:{
            fprintf(stderr,"ReturnK:\n");
            tree->check = 1;
            if(tree->child[0]->kind.exp == CALLfunctionK) cGen(tree->child[0]);
            emit("RETURN", "_", "_", tree->child[0]->attr.name);
            return;
        }

        default:
            fprintf(stderr, "Erro Stmt: %s\n%d\n", tree->attr.name, tree->kind.stmt);
    }
}

void cGen(TreeNode *tree) {
    //fprintf(stderr,"cGen talves nulo\n");
    if (tree == NULL) return;
    //fprintf(stderr,"cGen nao nulo\n");
    int i =0;
    switch (tree->nodekind) {
        case StmtK:
            fprintf(stderr,"CALL GENSTMT\n");
            genStmt(tree);
            break;
        case ExpK:
            fprintf(stderr,"CALL GENEXP: \n");
            genExp(tree);
            break;
    }
    
    for (int i = 0; i < MAXCHILDREN; i++){
        if(tree->check == 0){
            cGen(tree->child[i]);
        }
    }
    if(tree->param == 0) cGen(tree->sibling);
}


void emit(char *op, char *arg1, char *arg2, char* arg3) {
    strcpy(codecodecode[emitLoc].op, op);
    strcpy(codecodecode[emitLoc].arg1, arg1);
    strcpy(codecodecode[emitLoc].arg2, arg2);
    strcpy(codecodecode[emitLoc].arg3, arg3);
    codecodecode[emitLoc].line = emitLoc;
    emitLoc++;
    
}

void printIntermediateCode() {
    for (int i = 0; i < emitLoc; ++i) {
        printf("%3d: (%s, %s, %s, %s)\n", i,
               codecodecode[i].op,
               codecodecode[i].arg1,
               codecodecode[i].arg2,
               codecodecode[i].arg3);
    }
    //emit('HALT', '-','-','-');
    //strcpy(codecodecode[emitLoc].op, NULL);
}

char* get_reg(int a) {
    char* temp = malloc(32 * sizeof(char));  // suficiente para "R" + número + '\0'
    if (temp == NULL) {
        fprintf(stderr, "Erro de alocação\n");
        exit(1);
    }
    sprintf(temp, "R%d", a);
    return temp;
}

char* newLabel() {
    char *label = (char*)malloc(5 * sizeof(char));
    sprintf(label, "L%d", labelCount++);
    return label;
}

char* int_to_string(int a) {
    char* temp = malloc(32 * sizeof(char));  // suficiente para "R" + número + '\0'
    if (temp == NULL) {
        fprintf(stderr, "Erro de alocação\n");
        exit(1);
    }
    sprintf(temp, "%d", a);
    return temp;
}