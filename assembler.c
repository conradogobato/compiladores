#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Prototipos de Funcoes Auxiliares Privadas ---

// --- Implementacoes de Funcoes Publicas ---
void generate_assembly(const Quad* instructions, FILE* output_file) {
    if (!instructions || !output_file) {
        fprintf(stderr, "Erro: Argumentos invalidos para generate_assembly.\n");
        return;
    }

    // --- Boilerplate do Assembly ---

    int i = 0;
    // --- Traduzir cada instrucao ---

    // CORRECAO: O loop deve verificar se a operacao e NULL.
    // Esta e a maneira padrao e segura de percorrer um array de structs
    // sem um contador pre-calculado.
    for(i = 0; i < emitLoc; i++) {
        const char* op_str = instructions[i].op; // Nao precisa mais da verificacao ternaria aqui
        const char* arg1_str = instructions[i].arg1 ? instructions[i].arg1 : "-";
        const char* arg2_str = instructions[i].arg2 ? instructions[i].arg2 : "-";
        const char* arg3_str = instructions[i].arg3 ? instructions[i].arg3 : "-";

        //fprintf( stderr, "# IR %d: (%s, %s, %s, %s)\n", instructions[i].line, arg1_str, arg2_str, arg3_str);
        
        // Reativando a chamada para a funcao de traducao
        translate_instruction(&instructions[i], output_file);

        // CRITICO: Nao se esqueca de incrementar o indice!
    }

    // --- Adicionar qualquer codigo assembly final ---
    fprintf( stderr, "\n# Saida padrao do programa\n");
    fprintf( stderr, "exit:\n");
    fprintf( stderr, "    mov $60, %%rax\n");
    fprintf( stderr, "    xor %%rdi, %%rdi\n");
    fprintf( stderr, "    syscall\n");
}


// --- Implementacoes de Funcoes Auxiliares Privadas ---
void translate_instruction(const Quad* instr, FILE* out) {
    // O nucleo do assembler, agora gerando codigo MIPS.
    // NOTA: Os registos ($t0, $t1, etc.) sao placeholders. Um alocador de registos real e necessario.

    if (strcmp(instr->op, "FUNC") == 0) {
        if (instr->arg1) {
            fprintf( stderr, "%s:\n", instr->arg1);
            // Prologo de funcao MIPS
            fprintf( stderr, "    # Prologo\n");
            fprintf( stderr, "    addiu $sp, $sp, -8  # Aloca espaco na pilha\n");
            fprintf( stderr, "    sw $ra, 4($sp)      # Guarda o endereco de retorno\n");
            fprintf( stderr, "    sw $fp, 0($sp)      # Guarda o frame pointer antigo\n");
            fprintf( stderr, "    move $fp, $sp       # Estabelece o novo frame pointer\n");
        }
    } else if (strcmp(instr->op, "END FUNCTION") == 0) {
        // Epilogo de funcao MIPS
        fprintf( stderr, "    # Epilogo\n");
        fprintf( stderr, "    move $sp, $fp       # Restaura o stack pointer\n");
        fprintf( stderr, "    lw $fp, 0($sp)      # Restaura o frame pointer antigo\n");
        fprintf( stderr, "    lw $ra, 4($sp)      # Restaura o endereco de retorno\n");
        fprintf( stderr, "    addiu $sp, $sp, 8   # Liberta o espaco na pilha\n");
        fprintf( stderr, "    jr $ra              # Retorna para a funcao que chamou\n\n");
    } else if (strcmp(instr->op, "LOAD") == 0) {
        // lw (Load Word)
        // TODO: O offset -4 e o registo $t0 sao placeholders.
        fprintf( stderr, "    lw $t0, -4($fp)     # Placeholder\n");
    } else if (strcmp(instr->op, "LOADI") == 0) {
        // li (Load Immediate)
        if (instr->arg1) {
            fprintf( stderr, "    li $t1, %s          # Placeholder\n", instr->arg1);
        }
    } else if (strcmp(instr->op, "STORE") == 0) {
        // sw (Store Word)
        // TODO: O offset -8 e o registo $t0 sao placeholders.
        fprintf( stderr, "    sw $t0, -8($fp)     # Placeholder\n");
    } else if (strcmp(instr->op, "EQ") == 0) {
        // seq (Set on Equal)
        // TODO: Os registos sao placeholders.
        fprintf( stderr, "    seq $t2, $t0, $t1   # R2 = (R0 == R1)\n");
    } else if (strcmp(instr->op, "JUMP_FALSE") == 0) {
        // beqz (Branch on Equal to Zero)
        if (instr->arg3) {
            fprintf( stderr, "    beqz $t2, %s      # Salta se R2 for falso (0)\n", instr->arg3);
        }
    } else if (strcmp(instr->op, "JUMP") == 0) {
        // j (Jump)
        if (instr->arg3) {
            fprintf( stderr, "    j %s\n", instr->arg3);
        }
    } else if (strcmp(instr->op, "LABEL") == 0) {
        if (instr->arg3) {
            fprintf( stderr, "%s:\n", instr->arg3);
        }
    } else if (strcmp(instr->op, "PARAM") == 0) {
        // Em MIPS, os parametros sao passados nos registos $a0, $a1, ...
        // TODO: Isto precisa de um sistema para controlar qual registo de argumento usar.
        fprintf( stderr, "    move $a0, $t0       # Placeholder: assume que o parametro esta em $t0 e e o primeiro\n");
    } else if (strcmp(instr->op, "CALL") == 0) {
        // jal (Jump and Link)
        if (instr->arg1) {
            fprintf( stderr, "    jal %s\n", instr->arg1);
        }
    } else if (strcmp(instr->op, "SUB") == 0) {
        // sub (Subtract)
        fprintf( stderr, "    sub $t3, $t1, $t2   # Placeholder\n");
    } else if (strcmp(instr->op, "MUL") == 0) {
        // mul (Multiply)
        fprintf( stderr, "    mul $t3, $t1, $t2   # Placeholder\n");
    } else if (strcmp(instr->op, "DIV") == 0) {
        // div (Divide)
        fprintf( stderr, "    div $t1, $t2        # Placeholder\n");
        fprintf( stderr, "    mflo $t3            # Recupera o quociente para $t3\n");
    } else if (strcmp(instr->op, "RETURN") == 0) {
        // O valor de retorno deve ser movido para $v0
        fprintf( stderr, "    # TODO: Mover o valor de retorno (ex: de $t0) para $v0\n");
        fprintf( stderr, "    # move $v0, $t0\n");
        // O epilogo da funcao (gerado por END FUNCTION) fara o retorno.
    } else {
        if (instr->op) {
            fprintf( stderr, "    # Opcode nao tratado: %s\n", instr->op);
        }
    }
}

// Coloque esta função no seu assembler.c ou num ficheiro util.c

int get_register_number(const char *reg_name) {
    if (strcmp(reg_name, "$zero") == 0) return 0;
    if (strcmp(reg_name, "$at") == 0) return 1;
    if (strcmp(reg_name, "$v0") == 0) return 2;
    if (strcmp(reg_name, "$v1") == 0) return 3;
    if (strcmp(reg_name, "$a0") == 0) return 4;
    if (strcmp(reg_name, "$a1") == 0) return 5;
    // ... continue para todos os registadores ...
    if (strcmp(reg_name, "$t0") == 0) return 8;
    if (strcmp(reg_name, "$t1") == 0) return 9;
    // ... etc ...
    if (strcmp(reg_name, "$s0") == 0) return 16;
    // ... etc ...
    if (strcmp(reg_name, "$sp") == 0) return 29;
    if (strcmp(reg_name, "$fp") == 0) return 30;
    if (strcmp(reg_name, "$ra") == 0) return 31;

    // Se o nome não for encontrado, retorne um erro
    return -1;
}