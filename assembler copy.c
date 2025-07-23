#include "assembler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Estruturas de Dados e Globais para o Assembler ---

// Estrutura para rastrear a localização de uma variável (símbolo) na pilha.
typedef struct {
    char name[20];
    int offset; // Deslocamento a partir do Frame Pointer ($fp)
} SymbolLocation;

// Tabela de símbolos para a função atual e o deslocamento atual da pilha.
#define MAX_SYMBOLS 100
static SymbolLocation g_symbol_table[MAX_SYMBOLS];
static int g_symbol_count = 0;
static int g_current_stack_offset = 0; // Deslocamento negativo a partir do $fp
static int g_param_count = 0; // Contador para parâmetros de função

// --- Protótipos de Funções Auxiliares Privadas ---


// --- Implementações de Funções Públicas ---

void generate_assembly(const Quad* instructions, FILE* output_file, int instruction_count) {
    if (!instructions || !output_file) {
        fprintf(stderr, "Erro: Argumentos invalidos para generate_assembly.\n");
        return;
    }

    // --- Boilerplate do Assembly (início do arquivo) ---
    fprintf(output_file, ".data\n");
    fprintf(output_file, "newline: .asciiz \"\\n\"\n");
    fprintf(output_file, ".text\n");
    fprintf(output_file, ".globl main\n\n");


    // --- Traduzir cada instrução, tratando uma função de cada vez ---
    int i = 0;
    while (i < instruction_count) {
        if (strcmp(instructions[i].op, "FUNC") == 0) {
            // Encontramos o início de uma função.
            
            // 1. Pré-processamento para calcular o tamanho da pilha.
            int function_size = pre_process_function_stack(instructions, i);
            
            // 2. Gerar o código para a função.
            const char* func_name = instructions[i].arg1;
            char func_end_label[32];
            sprintf(func_end_label, "L_end_%s", func_name);

            // Traduzir a instrução FUNC (prólogo)
            translate_instruction(&instructions[i], output_file, func_end_label);
            
            // Ajustar o ponteiro da pilha com o tamanho calculado
            fprintf(output_file, "    addiu $sp, $sp, -%d   # Aloca %d bytes na pilha\n", function_size, function_size);
            
            // Mover frame pointer
            fprintf(output_file, "    sw $ra, %d($sp)       # Guarda o endereco de retorno\n", function_size - 4);
            fprintf(output_file, "    sw $fp, %d($sp)       # Guarda o frame pointer antigo\n", function_size - 8);
            fprintf(output_file, "    addiu $fp, $sp, %d    # Estabelece o novo frame pointer\n\n", function_size - 8);

            i++; // Pula a instrução FUNC

            // Traduzir o corpo da função
            while (i < instruction_count && strcmp(instructions[i].op, "END FUNCTION") != 0) {
                translate_instruction(&instructions[i], output_file, func_end_label);
                i++;
            }
            
            // Traduzir a instrução END FUNCTION (epílogo)
            if (i < instruction_count) {
                translate_instruction(&instructions[i], output_file, func_end_label);
            }
        }
        i++;
    }

    // --- Código de finalização do programa (exit syscall) ---
    fprintf(output_file, "\n# Finalizacao padrao do programa\n");
    fprintf(output_file, "exit_program:\n");
    fprintf(output_file, "    li $v0, 10          # Syscall para sair\n");
    fprintf(output_file, "    syscall\n");
}

// --- Implementações de Funções Auxiliares Privadas ---

/**
 * @brief Inicializa o contexto (tabela de símbolos, offsets) para uma nova função.
 */
static void initialize_function_context() {
    g_symbol_count = 0;
    g_current_stack_offset = 0; // Começa em 0, relativo ao $fp
}

/**
 * @brief Encontra o deslocamento de uma variável na pilha. Se não existir, cria uma nova entrada.
 * @param name O nome da variável.
 * @return O deslocamento em bytes a partir do $fp.
 */
static int find_or_create_symbol_offset(const char* name) {
    for (int i = 0; i < g_symbol_count; ++i) {
        if (strcmp(g_symbol_table[i].name, name) == 0) {
            return g_symbol_table[i].offset;
        }
    }
    // Símbolo não encontrado, é uma nova variável local.
    g_current_stack_offset -= 4; // Aloca 4 bytes para a nova variável
    strcpy(g_symbol_table[g_symbol_count].name, name);
    g_symbol_table[g_symbol_count].offset = g_current_stack_offset;
    g_symbol_count++;
    return g_current_stack_offset;
}

/**
 * @brief Converte um nome de registrador do IR (ex: "R5") para MIPS (ex: "$t5").
 */
static void get_temp_register_name(const char* ir_reg, char* mips_reg_buffer) {
    if (ir_reg && ir_reg[0] == 'R') {
        int reg_num = atoi(ir_reg + 1); // Pula o 'R' e converte para int
        sprintf(mips_reg_buffer, "$t%d", reg_num);
    } else {
        // Fallback ou erro
        sprintf(mips_reg_buffer, "$zero"); 
    }
}


/**
 * @brief Faz uma passagem pela função para descobrir todas as variáveis locais e calcular o tamanho da pilha.
 * @return O tamanho total necessário para a pilha da função.
 */
static int pre_process_function_stack(const Quad* instructions, int start_index) {
    initialize_function_context();
    int i = start_index + 1;
    // Adiciona parâmetros à tabela de símbolos (eles estão acima do $fp)
    // Esta parte requer conhecimento da convenção de chamada, por simplicidade, vamos focar nas locais.

    while (strcmp(instructions[i].op, "END FUNCTION") != 0) {
        if (strcmp(instructions[i].op, "STORE") == 0 || strcmp(instructions[i].op, "LOAD") == 0) {
            if(instructions[i].arg1[0] != 'R') { // Apenas para variáveis nomeadas, não registradores
                find_or_create_symbol_offset(instructions[i].arg1);
            }
        }
        i++;
    }
    // Tamanho total = (espaço para variáveis locais) + (espaço para $ra e $fp)
    int local_vars_size = -g_current_stack_offset;
    return local_vars_size + 8; // +8 para $ra e $fp
}


/**
 * @brief O núcleo do assembler, traduz uma única instrução Quad para Assembly MIPS.
 */
static void translate_instruction(const Quad* instr, FILE* out, const char* func_end_label) {
    char reg1[10], reg2[10], reg3[10];

    // Converte os nomes de registradores do IR para MIPS quando aplicável
    if (instr->arg1 && instr->arg1[0] == 'R') get_temp_register_name(instr->arg1, reg1);
    if (instr->arg2 && instr->arg2[0] == 'R') get_temp_register_name(instr->arg2, reg2);
    if (instr->arg3 && instr->arg3[0] == 'R') get_temp_register_name(instr->arg3, reg3);

    if (strcmp(instr->op, "FUNC") == 0) {
        fprintf(out, "\n%s:\n", instr->arg1);
        fprintf(out, "    # Prologo (tamanho da pilha calculado na pre-analise)\n");
        // A alocação real é feita na função `generate_assembly` que tem o tamanho
    } 
    else if (strcmp(instr->op, "END FUNCTION") == 0) {
        fprintf(out, "%s:\n", func_end_label);
        fprintf(out, "    # Epilogo\n");
        fprintf(out, "    move $sp, $fp         # Desfaz o frame, restaurando o ponteiro de pilha\n");
        fprintf(out, "    lw $fp, 0($sp)        # Restaura o frame pointer antigo\n");
        fprintf(out, "    lw $ra, 4($sp)        # Restaura o endereco de retorno\n");
        fprintf(out, "    addiu $sp, $sp, 8     # Liberta o espaco de $fp e $ra\n");
        fprintf(out, "    jr $ra                # Retorna para a funcao que chamou\n\n");
    } 
    else if (strcmp(instr->op, "LOAD") == 0) { // Carrega variável da pilha para um registrador
        int offset = find_or_create_symbol_offset(instr->arg1);
        fprintf(out, "    lw %s, %d($fp)     # Carrega '%s' para %s\n", reg3, offset, instr->arg1, reg3);
    } 
    else if (strcmp(instr->op, "LOADI") == 0) { // Carrega valor imediato para um registrador
        fprintf(out, "    li %s, %s            # Carrega imediato %s para %s\n", reg3, instr->arg1, instr->arg1, reg3);
    } 
    else if (strcmp(instr->op, "STORE") == 0) { // Armazena valor de um registrador na pilha
        int offset = find_or_create_symbol_offset(instr->arg1);
        fprintf(out, "    sw %s, %d($fp)     # Armazena %s em '%s'\n", reg3, offset, reg3, instr->arg1);
    } 
    else if (strcmp(instr->op, "EQ") == 0) { // Compara se R1 == R2, resultado em R3
        fprintf(out, "    seq %s, %s, %s      # %s = (%s == %s)\n", reg3, reg1, reg2, reg3, reg1, reg2);
    } 
    else if (strcmp(instr->op, "JUMP_FALSE") == 0) {
        fprintf(out, "    beqz %s, %s         # Salta para %s se %s for falso (0)\n", reg1, instr->arg3, instr->arg3, reg1);
    } 
    else if (strcmp(instr->op, "JUMP") == 0) {
        fprintf(out, "    j %s\n", instr->arg3);
    } 
    else if (strcmp(instr->op, "LABEL") == 0) {
        fprintf(out, "%s:\n", instr->arg3);
    } 
    else if (strcmp(instr->op, "PARAM") == 0) {
        // Convenção MIPS: $a0, $a1, $a2, $a3 para os primeiros 4 parâmetros
        fprintf(out, "    move $a%d, %s        # Prepara parametro %d\n", g_param_count, reg1, g_param_count + 1);
        g_param_count++;
    } 
    else if (strcmp(instr->op, "CALL") == 0) {
        fprintf(out, "    jal %s\n", instr->arg1);
        fprintf(out, "    move %s, $v0         # Captura valor de retorno em %s\n", reg3, reg3);
        g_param_count = 0; // Reseta contador de parâmetros após a chamada
    } 
    else if (strcmp(instr->op, "SUB") == 0) {
        fprintf(out, "    sub %s, %s, %s      # %s = %s - %s\n", reg3, reg1, reg2, reg3, reg1, reg2);
    } 
    else if (strcmp(instr->op, "MUL") == 0) {
        fprintf(out, "    mul %s, %s, %s      # %s = %s * %s\n", reg3, reg1, reg2, reg3, reg1, reg2);
    } 
    else if (strcmp(instr->op, "DIV") == 0) {
        fprintf(out, "    div %s, %s\n", reg1, reg2);
        fprintf(out, "    mflo %s              # Quociente da divisao para %s\n", reg3, reg3);
    } 
    else if (strcmp(instr->op, "RETURN") == 0) {
        // Move o valor de retorno (que está em um registrador temporário) para $v0
        fprintf(out, "    move $v0, %s         # Prepara valor de retorno\n", reg3);
        fprintf(out, "    j %s               # Pula para o epilogo da funcao\n", func_end_label);
    }
    // Adicione um caso especial para a chamada de output
    else if (strcmp(instr->op, "output") == 0) { // Assumindo uma instrução especial para output
        fprintf(out, "    move $a0, %s         # Move valor para argumento de print\n", reg1);
        fprintf(out, "    li $v0, 1             # Syscall para imprimir inteiro\n");
        fprintf(out, "    syscall\n");
        fprintf(out, "    la $a0, newline       # Carrega endereço de nova linha\n");
        fprintf(out, "    li $v0, 4             # Syscall para imprimir string\n");
        fprintf(out, "    syscall\n");
    }
    else {
        if (instr->op && strlen(instr->op) > 0) {
            fprintf(out, "    # Opcode nao tratado: %s\n", instr->op);
        }
    }
}