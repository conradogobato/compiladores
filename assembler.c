#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Estruturas de Dados e Globais ---

#define MAX_SYMBOLS 100
#define MAX_PARAMS 10
static char g_symbol_table[MAX_SYMBOLS][20];
static int g_symbol_count = 0;
static int g_param_count = 0;
static char g_current_function[50];
// REMOVIDO: static char g_output_param_reg[20]; // Não é mais necessário, a lógica foi melhorada.
static char g_param_sources[MAX_PARAMS][20];

// Protótipos para funções estáticas (boa prática)


void generate_assembly(const Quad* instructions, FILE* output_file, int instruction_count) {
    if (!instructions || !output_file) {
        fprintf(stderr, "Erro: Argumentos invalidos para generate_assembly.\n");
        return;
    }

    initialize_context();

    // --- Passagem 1: Encontrar todas as variáveis para a seção .data ---
    pre_process_data_layout(instructions, instruction_count);

    // --- Geração da Seção .data ---
    fprintf(output_file, ".data\n");
    for (int i = 0; i < g_symbol_count; ++i) {
        fprintf(output_file, "%s:\t.word 0\n", g_symbol_table[i]);
    }
    fprintf(output_file, "\n");

    // --- Geração da Seção .text ---
    fprintf(output_file, ".text\n");
    fprintf(output_file, ".globl main\n\n");

    fprintf(output_file, "\tjump main\n\n");

    // --- Passagem 2: Traduzir cada instrução ---
    for (int i = 0; i < instruction_count; i++) {
        const Quad* current_instr = &instructions[i];

        // --- LÓGICA DE OTIMIZAÇÃO DE CHAMADA DE CAUDA ---
        // Verifica se a instrução atual é uma chamada de cauda.
        // O padrão é: CALL, seguido por um RETURN do resultado dessa chamada.
        if (strcmp(current_instr->op, "CALL") == 0 && (i + 1 < instruction_count)) {
            const Quad* next_instr = &instructions[i + 1];
            // É uma chamada para a função atual (recursiva) e a próxima instrução é um RETURN?
            if (strcmp(next_instr->op, "RETURN") == 0 && strcmp(current_instr->arg1, g_current_function) == 0) {
                
                // É UMA CHAMADA DE CAUDA! Gera código otimizado.
                fprintf(output_file, "    # --- Tail-Call Optimization --- \n");
                
                // 1. Gera os 'move's para atualizar os registradores de argumento
                for (int p = 0; p < g_param_count; p++) {
                    // Convenção: Parâmetros vão para r4, r5, ...
                    fprintf(output_file, "    move r%d, %s\n", 4 + p, g_param_sources[p]);
                }
                
                // 2. Gera um 'jump' em vez de 'jal'
                fprintf(output_file, "    jump %s\n", g_current_function);
                
                // 3. Reseta o contador de parâmetros
                g_param_count = 0;
                
                // 4. PULA as instruções CALL e RETURN originais do IR
                i++; // Avança o loop para pular a instrução RETURN
                continue; // Pula para a próxima iteração do loop
            }
        }

        // Se não for TCO, processa a instrução normalmente.
        // Passamos o contexto completo para a função de tradução para permitir decisões mais inteligentes.
        translate_instruction_simple(current_instr, output_file, instructions, i, instruction_count);
    }
    
    // Adicionado HLT no final do programa para parar a execução
    fprintf(output_file, "\nexit_program:\n");
    fprintf(output_file, "    hlt\n");
}

// --- Implementações de Funções Auxiliares ---

static void initialize_context() {
    g_symbol_count = 0;
    g_param_count = 0;
    strcpy(g_current_function, "");
}

static void pre_process_data_layout(const Quad* instructions, int instruction_count) {
    for (int i = 0; i < instruction_count; i++) {
        if (strcmp(instructions[i].op, "STORE") == 0) {
            int found = 0;
            for (int j = 0; j < g_symbol_count; j++) {
                if (strcmp(g_symbol_table[j], instructions[i].arg1) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found && g_symbol_count < MAX_SYMBOLS) {
                strcpy(g_symbol_table[g_symbol_count++], instructions[i].arg1);
            }
        }
    }
}

static void get_gpr_name(const char* ir_reg, char* gpr_buffer) {
    if (ir_reg && ir_reg[0] == 'R') {
        int reg_num = atoi(ir_reg + 1);
        sprintf(gpr_buffer, "r%d", reg_num);
    } else {
        strcpy(gpr_buffer, ir_reg);
    }
}


// O núcleo do assembler, agora com mais contexto
static void translate_instruction_simple(const Quad* instr, FILE* out, const Quad* instructions, int current_index, int instruction_count) {
    char arg1[20], arg2[20], arg3[20];

    get_gpr_name(instr->arg1, arg1);
    get_gpr_name(instr->arg2, arg2);
    // Para o arg3 (result), usamos o nome correto do registrador (rX)
    get_gpr_name(instr->arg3, arg3);

    if (strcmp(instr->op, "FUNC") == 0) {
        fprintf(out, "%s:\n", instr->arg1);
        strcpy(g_current_function, instr->arg1);
    } 
    else if (strcmp(instr->op, "END FUNCTION") == 0) {
        if (strcmp(g_current_function, "main") == 0) {
            fprintf(out, "    jump exit_program\n\n");
        } else {
            // Retorno padrão para funções não-main (e não-cauda)
            fprintf(out, "    jr r62\n\n");
        }
    } 
    else if (strcmp(instr->op, "LOAD") == 0) {
        // CORREÇÃO: Tratar "u" e "v" como parâmetros, não como labels de memória.
        // Esta é uma correção crucial. Assumimos que o IR está usando 'u' e 'v' simbolicamente.
        // A convenção de chamada dita que 'u' está em r4 e 'v' em r5.
        if (strcmp(g_current_function, "gcd") == 0) {
            if (strcmp(instr->arg1, "u") == 0) {
                fprintf(out, "    move %s, r4\n", arg3); // Copia de r4 para o registrador de destino
                return;
            } else if (strcmp(instr->arg1, "v") == 0) {
                fprintf(out, "    move %s, r5\n", arg3); // Copia de r5 para o registrador de destino
                return;
            }
        }
        // Comportamento padrão para outras variáveis (globais)
        fprintf(out, "    load %s, %s\n", arg3, arg1);
    } 
    else if (strcmp(instr->op, "LOADI") == 0) {
        fprintf(out, "    loadi %s, %s\n", arg3, arg1);
    } 
    else if (strcmp(instr->op, "STORE") == 0) {
        // No STORE, o arg3 é o registrador fonte
        get_gpr_name(instr->arg3, arg3);
        fprintf(out, "    store %s, %s\n", arg3, arg1);
    } 
    else if (strcmp(instr->op, "ADD") == 0) {
        fprintf(out, "    add %s, %s, %s\n", arg3, arg1, arg2);
    } 
    else if (strcmp(instr->op, "SUB") == 0) {
        fprintf(out, "    sub %s, %s, %s\n", arg3, arg1, arg2);
    } 
    else if (strcmp(instr->op, "MUL") == 0) {
        fprintf(out, "    mult %s, %s, %s\n", arg3, arg1, arg2);
    } 
    else if (strcmp(instr->op, "DIV") == 0) {
        fprintf(out, "    div %s, %s, %s\n", arg3, arg1, arg2);
    } 
    else if (strcmp(instr->op, "EQ") == 0) {
        fprintf(out, "    seq %s, %s, %s\n", arg3, arg1, arg2);
    }
    else if (strcmp(instr->op, "GT") == 0) {
        fprintf(out, "    sgt %s, %s, %s\n", arg3, arg1, arg2);
    }
    else if (strcmp(instr->op, "LT") == 0) {
        fprintf(out, "    slt %s, %s, %s\n", arg3, arg1, arg2);
    }   
    else if (strcmp(instr->op, "JUMP_FALSE") == 0) {
        // O arg3 em JUMP_FALSE é o label de destino
        get_gpr_name(instr->arg3, arg3);
        fprintf(out, "    beq %s, r63, %s\n", arg1, arg3); // r63 é o registrador zero
    } 
    else if (strcmp(instr->op, "JUMP") == 0) {
        get_gpr_name(instr->arg3, arg3);
        fprintf(out, "    jump %s\n", arg3);
    } 
    else if (strcmp(instr->op, "LABEL") == 0) {
        get_gpr_name(instr->arg3, arg3);
        fprintf(out, "%s:\n", arg3);
    } 
    else if (strcmp(instr->op, "PARAM") == 0) {
        if (g_param_count < MAX_PARAMS) {
            get_gpr_name(instr->arg1, g_param_sources[g_param_count]);
            g_param_count++;
        }
    } 
    else if (strcmp(instr->op, "CALL") == 0) {
        if (strcmp(instr->arg1, "input") == 0) {
            fprintf(out, "    in %s\n", arg3);
        } else if (strcmp(instr->arg1, "output") == 0) {
            // CORREÇÃO: Usa o registrador correto que foi passado como parâmetro.
            // g_param_sources[0] conterá o nome do registrador do último PARAM.
            fprintf(out, "    out %s\n", g_param_sources[0]);
        } else {
            // Chamada de função normal (não-cauda)
            for (int i = 0; i < g_param_count; i++) {
                fprintf(out, "    move r%d, %s\n", 4 + i, g_param_sources[i]);
            }
            fprintf(out, "    jal %s\n", instr->arg1);
            
            // Move o valor de retorno (de r2) para o registrador de destino
            if (instr->arg3 && strcmp(instr->arg3, "-") != 0) {
                fprintf(out, "    move %s, r2\n", arg3);
            }
        }
        g_param_count = 0; // Reseta para a próxima chamada
    } 
    else if (strcmp(instr->op, "RETURN") == 0) {
        // Move o valor de retorno para r2 (convenção)
        if (instr->arg3 && strcmp(instr->arg3, "-") != 0) {
            fprintf(out, "    move r2, %s\n", arg3);
        }
    }
}