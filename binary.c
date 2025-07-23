#include "binary.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- Definições da Arquitetura Customizada ---

// --- Tabelas de Mapeamento e Globais ---

#define MAX_LABELS 100
#define MAX_ARGS 4

static LabelInfo g_label_table[MAX_LABELS];
static int g_label_count = 0;

static const RegisterInfo g_register_map[] = {
    {"$zero", 0}, {"r0", 0}, {"$at", 1}, {"r1", 1}, {"$v0", 2}, {"r2", 2},
    {"$v1", 3}, {"r3", 3}, {"$a0", 4}, {"r4", 4}, {"$a1", 5}, {"r5", 5},
    {"$a2", 6}, {"r6", 6}, {"$a3", 7}, {"r7", 7}, {"$t0", 8}, {"r8", 8},
    {"$t1", 9}, {"r9", 9}, {"$t2", 10}, {"r10", 10},{"$t3", 11}, {"r11", 11},
    {"$t4", 12}, {"r12", 12},{"$t5", 13}, {"r13", 13},{"$t6", 14}, {"r14", 14},
    {"$t7", 15}, {"r15", 15},{"$s0", 16}, {"r16", 16},{"$s1", 17}, {"r17", 17},
    {"$s2", 18}, {"r18", 18},{"$s3", 19}, {"r19", 19},{"$s4", 20}, {"r20", 20},
    {"$s5", 21}, {"r21", 21},{"$s6", 22}, {"r22", 22},{"$s7", 23}, {"r23", 23},
    {"$t8", 24}, {"r24", 24},{"$t9", 25}, {"r25", 25},{"$k0", 26}, {"r26", 26},
    {"$k1", 27}, {"r27", 27},{"$gp", 28}, {"r28", 28},{"$sp", 29}, {"r29", 29},
    {"$fp", 30}, {"r30", 30},{"$ra", 31}, {"r31", 31}, {"r63", 63}
};
const int NUM_REGISTERS = sizeof(g_register_map) / sizeof(g_register_map[0]);

static const OpcodeInfo g_opcode_table[] = {
    {"add",     0b000000, FORMAT_1},
    {"addu",    0b000000, FORMAT_1},
    {"addiu",   0b000001, FORMAT_2}, {"addi", 0b000001, FORMAT_2},
    {"sub",     0b000010, FORMAT_1},
    {"subu",    0b000010, FORMAT_1},
    {"subi",    0b000011, FORMAT_2},
    {"div",     0b000100, FORMAT_1},
    {"mult",    0b000110, FORMAT_1},
    {"or",      0b001000, FORMAT_1},
    {"and",     0b001001, FORMAT_1},
    {"xor",     0b001010, FORMAT_1},
    {"not",     0b001011, FORMAT_3},
    {"j",       0b001110, FORMAT_4}, {"jump", 0b001110, FORMAT_4},
    {"jr",      0b001111, FORMAT_5},
    {"jal",     0b010000, FORMAT_6},
    {"beq",     0b010001, FORMAT_7},
    {"bne",     0b010010, FORMAT_7},
    {"move",    0b010011, FORMAT_3},
    {"li",      0,        FORMAT_PSEUDO},
    {"store",   0b010110, FORMAT_8},
    {"in",      0b010111, FORMAT_9},
    {"out",     0b011000, FORMAT_9},
    {"nop",     0b011001, FORMAT_10},
    {"hlt",     0b011010, FORMAT_10},
    {"sgt",     0b011011, FORMAT_1},
    {"slt",     0b011100, FORMAT_1},
    {"seq",     0b011101, FORMAT_1},
    {"load",    0b010100, FORMAT_8},
    {"loadi",   0b010101, FORMAT_8},
    {"beqz",    0b010001, FORMAT_7},
    {"lw",      0,        FORMAT_UNSUPPORTED},
    {"sw",      0,        FORMAT_UNSUPPORTED},
    {"mflo",    0,        FORMAT_UNSUPPORTED},
    {"la",      0,        FORMAT_UNSUPPORTED},
    {"syscall", 0,        FORMAT_UNSUPPORTED}
};
const int NUM_OPCODES = sizeof(g_opcode_table) / sizeof(g_opcode_table[0]);

// --- Funções Auxiliares ---
static const OpcodeInfo* find_opcode_info(const char* mnemonic) {
    for (int i = 0; i < NUM_OPCODES; ++i) {
        if (strcasecmp(g_opcode_table[i].name, mnemonic) == 0) return &g_opcode_table[i];
    }
    return NULL;
}
static int get_register_number(const char* reg_str) {
    if (!reg_str || reg_str[0] == '\0') return -1;
    for (int i = 0; i < NUM_REGISTERS; ++i) {
        if (strcmp(g_register_map[i].name, reg_str) == 0) return g_register_map[i].number;
    }
    if (tolower(reg_str[0]) == 'r' && isdigit(reg_str[1])) return atoi(reg_str + 1);
    return -1;
}
static uint32_t get_label_address(const char* label_name) {
    for (int i = 0; i < g_label_count; ++i) {
        if (strcmp(g_label_table[i].name, label_name) == 0) return g_label_table[i].address;
    }
    fprintf(stderr, "ERROR: Label '%s' not found.\n", label_name);
    return 0;
}
uint32_t build_instruction(uint8_t opcode, int f1, int v1, int f2, int v2, int f3, int v3) {
    uint32_t instruction = 0;
    instruction |= ((uint32_t)opcode << 26);
    if (f1 >= 0) instruction |= ((uint32_t)v1 << f1);
    if (f2 >= 0) instruction |= ((uint32_t)v2 << f2);
    if (f3 >= 0) instruction |= ((uint32_t)v3 << f3);
    return instruction;
}
void parse_line(char* line, char* mnemonic, char args[MAX_ARGS][50], int* arg_count) {
    *arg_count = 0;
    mnemonic[0] = '\0';
    for(int i = 0; i < MAX_ARGS; i++) args[i][0] = '\0';
    char* comment = strchr(line, '#');
    if (comment) *comment = '\0';
    char* token = strtok(line, " \t\n,()");
    if (!token) return;
    strcpy(mnemonic, token);
    while ((token = strtok(NULL, " \t\n,()")) != NULL) {
        if (*arg_count < MAX_ARGS) {
            strcpy(args[*arg_count], token);
            (*arg_count)++;
        }
    }
}
void write_binary_string(FILE* file, uint32_t word) {
    for (int i = 31; i >= 0; i--) {
        fputc(((word >> i) & 1) ? '1' : '0', file);
    }
    fprintf(file, "\n");
}

uint32_t assemble_instruction(const OpcodeInfo* op, char args[MAX_ARGS][50], int arg_count, uint32_t current_address) {
    int rd=0, rs=0, rt=0;
    long imm=0;
    uint32_t addr=0;

    switch (op->format) {
        case FORMAT_1: // op rs, rt, rd
            rs = get_register_number(args[2]);
            rt = get_register_number(args[0]);
            rd = get_register_number(args[1]);
            return build_instruction(op->opcode, 20, rd, 14, rs, 8, rt);
        case FORMAT_2: // op rd, rs, imm (ordem MIPS)
            rd = get_register_number(args[0]);
            rs = get_register_number(args[1]);
            imm = strtol(args[2], NULL, 0);
            return build_instruction(op->opcode, 20, rd, 14, rs, 0, imm & 0x3FFF);
        case FORMAT_3: // op rd, rs (ordem MIPS)
            rd = get_register_number(args[1]);
            rs = get_register_number(args[0]);
            if (rs == rd) {
                return 0; // Retorna 0, que será tratado como um NOP / instrução pulada.
            }
            return build_instruction(op->opcode, 20, rd, 14, rs, -1, 0);
        case FORMAT_4: // jump addr
            addr = get_label_address(args[0]);
            return build_instruction(op->opcode, 0, addr & 0x03FFFFFF, -1, 0, -1, 0);
        case FORMAT_5: // jr rs
            rs = get_register_number(args[0]);
            return build_instruction(op->opcode, 20, rs, -1, 0, -1, 0);
        case FORMAT_6: // jal addr
            rd = get_register_number("r62"); 
            addr = get_label_address(args[0]);
            return build_instruction(op->opcode, 20, rd, 0, addr & 0x000FFFFF, -1, 0);
        case FORMAT_7: // beq rs, rt, offset
            rs = get_register_number(args[0]);
            rt = get_register_number(args[1]);
            uint32_t target_addr = get_label_address(args[2]);
            // CORREÇÃO: Cálculo de offset para PC que incrementa de 1 em 1
            int32_t offset = target_addr - (current_address + 1);
            return build_instruction(op->opcode, 20, rt, 14, rs, 0, offset & 0x3FFF);
        case FORMAT_8: 
            rd = get_register_number(args[0]);
            if (strcasecmp(op->name, "loadi") == 0) {
                // Se for 'loadi', o segundo argumento é SEMPRE um número.
                imm = strtol(args[1], NULL, 0);
                return build_instruction(op->opcode, 20, rd, 0, imm & 0x000FFFFF, -1, 0);
            } else {
                // Se for 'load' ou 'store', o segundo argumento é SEMPRE um label.
                addr = get_label_address(args[1]);
                if(addr == -1) { fprintf(stderr, "ERROR: Label '%s' not found.\n", args[1]); return 0;}
                return build_instruction(op->opcode, 20, rd, 0, addr & 0x000FFFFF, -1, 0);
            }
        case FORMAT_9: // in/out rd
            rd = get_register_number(args[0]);
            return build_instruction(op->opcode, 20, rd, -1, 0, -1, 0);
        case FORMAT_10: // nop, hlt
            return build_instruction(op->opcode, -1, 0, -1, 0, -1, 0);
        case FORMAT_PSEUDO:
            if (strcasecmp(op->name, "li") == 0) {
                const OpcodeInfo* loadi_op = find_opcode_info("loadi");
                if(loadi_op) {
                    rd = get_register_number(args[0]);
                    imm = strtol(args[1], NULL, 0);
                    return build_instruction(loadi_op->opcode, 20, rd, 0, imm & 0x000FFFFF, -1, 0);
                }
            }
            break;
        case FORMAT_UNSUPPORTED:
            fprintf(stderr, "WARNING: MIPS instruction '%s' is not supported by the custom architecture. Skipping.\n", op->name);
            return 0; 
    }
    return 0; // NOP
}

int assemble_to_binary(const char* input_assembly_filename, const char* output_binary_filename) {
    FILE* infile = fopen(input_assembly_filename, "r");
    if (!infile) {
        perror("Error opening input file");
        return -1;
    }

    g_label_count = 0;
    char line[256];
    
    // --- Passagem 1: Mapear Labels ---
    // ### INÍCIO DA CORREÇÃO DEFINITIVA ###
    uint32_t data_address_counter = 0;
    uint32_t text_address_counter = 0;
    int in_text_section = 0; // Assume que o arquivo começa na seção de dados

    while (fgets(line, sizeof(line), infile)) {
        char clean_line[256];
        strcpy(clean_line, line);
        
        char* comment = strchr(clean_line, '#');
        if (comment) *comment = '\0';
        
        char* l_ptr = clean_line;
        while(isspace((unsigned char)*l_ptr)) l_ptr++;

        if (*l_ptr == '\0') continue; // Pula linhas vazias
        
        if (strncmp(l_ptr, ".text", 5) == 0) { in_text_section = 1; continue; }
        if (strncmp(l_ptr, ".data", 5) == 0) { in_text_section = 0; continue; }
        
        char* content_ptr = l_ptr;
        char* colon = strchr(l_ptr, ':');
        
        if (colon) {
            *colon = '\0'; // Isola o nome do label
            if (g_label_count < MAX_LABELS) {
                strcpy(g_label_table[g_label_count].name, l_ptr);
                g_label_table[g_label_count].address = in_text_section ? text_address_counter : data_address_counter;
                g_label_count++;
            }
            content_ptr = colon + 1; // Avança o ponteiro para depois do label
        }
        
        // Verifica se a parte restante da linha contém algo que ocupa espaço
        l_ptr = content_ptr;
        while(isspace((unsigned char)*l_ptr)) l_ptr++;
        
        if (*l_ptr != '\0') {
            if (*l_ptr != '\0') {
                if (in_text_section) {
                    char mnemonic[20];
                    char args[MAX_ARGS][50];
                    int arg_count;

                    // Parseamos a linha completa para poder inspecionar os argumentos
                    parse_line(l_ptr, mnemonic, args, &arg_count);

                    if (strlen(mnemonic) > 0 && find_opcode_info(mnemonic) != NULL) {
                        int should_count = 1; // Começamos assumindo que a instrução será contada

                        // APLICAMOS A MESMA LÓGICA DE OTIMIZAÇÃO DA PASSAGEM 2
                        if (strcasecmp(mnemonic, "move") == 0 && arg_count == 2) {
                            int rs = get_register_number(args[0]);
                            int rd = get_register_number(args[1]);
                            if (rs == rd) {
                                should_count = 0; // É um move redundante, NÃO CONTE!
                            }
                        }

                        if (should_count) {
                            text_address_counter++; // Só incrementa se não for otimizada
                        }
                    }
                } else { // Seção .data
                    if (strstr(l_ptr, ".word") != NULL) {
                        data_address_counter++;
                    }
                }
            }
        }
    }
    // ### FIM DA CORREÇÃO ###
    fclose(infile);

    // --- Passagem 2: Gerar Código Binário ---
    infile = fopen(input_assembly_filename, "r");
    FILE* outfile = fopen(output_binary_filename, "w");
    if (!infile || !outfile) {
        perror("Error opening files for pass 2");
        if (infile) fclose(infile);
        if (outfile) fclose(outfile);
        return -1;
    }
    
    uint32_t current_address = 0;
    in_text_section = 0; // Reseta para a Passagem 2
    // ...
    char line_buffer[256]; 

    while (fgets(line_buffer, sizeof(line_buffer), infile)) {
        // CORRETO: Usando 'line_buffer'
        if (strstr(line_buffer, ".text")) { in_text_section = 1; continue; }
        if (strstr(line_buffer, ".data")) { in_text_section = 0; continue; }
        if (!in_text_section) continue;
    // ...

        char mnemonic[20];
        char args[MAX_ARGS][50];
        int arg_count;

        // ======================= INÍCIO DA CORREÇÃO DE ROBUSTEZ =======================
        char line_copy[256];
        strcpy(line_copy, line_buffer); // Trabalha em uma cópia para proteger a original

        char* line_to_parse = line_copy;
        char* colon = strchr(line_to_parse, ':');
        if(colon) {
            line_to_parse = colon + 1; // Pula o label, se houver
        }
        
        // Ignora linhas que são apenas espaços em branco após o label
        char* temp_ptr = line_to_parse;
        while(isspace((unsigned char)*temp_ptr)) temp_ptr++;
        if (*temp_ptr == '\0' || *temp_ptr == '#') {
            continue;
        }
       
        // A função parse_line agora opera em uma cópia segura
        parse_line(line_to_parse, mnemonic, args, &arg_count);

        if (strlen(mnemonic) == 0 || mnemonic[0] == '.') {
            continue;
        }
        // ======================== FIM DA CORREÇÃO DE ROBUSTEZ =========================

        
        const OpcodeInfo* op = find_opcode_info(mnemonic);
        if (!op) {
             fprintf(stderr, "WARNING: Unknown mnemonic '%s'. Skipping.\n", mnemonic);
            continue;
        }
        
        uint32_t instruction_word = assemble_instruction(op, args, arg_count, current_address);

        if (instruction_word != 0 || strcasecmp(op->name, "nop") == 0) {
            write_binary_string(outfile, instruction_word);
            current_address++;
        }
    }

    fclose(infile);
    fclose(outfile);
    printf("Assembly successful. Text-binary code written to %s.\n", output_binary_filename);
    return 0;
}