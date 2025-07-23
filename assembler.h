#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include "globals.h"
#include "symtab.h"
#include "cgen.h"

// --- Data Structures for Intermediate Representation ---

// Enum to represent the different operations in the intermediate code

// Struct to represent a single line of the intermediate code
// This is a simplified representation based on your example.
// You might need to adjust it to fit your actual data structure.
typedef struct {
    int line;
    char* op;
    char* arg1;
    char* arg2;
    char* result;
} IntermediateInstruction;


// --- Public Function Prototypes ---

/**
 * @brief Translates an array of intermediate instructions into assembly code.
 *
 * This is the main function of the assembler. It takes a list of intermediate
 * instructions, processes them one by one, and writes the corresponding
 * assembly code to the specified output file.
 *
 * @param instructions An array of IntermediateInstruction structs.
 * @param instruction_count The number of instructions in the array.
 * @param output_file A file pointer to the output file where the assembly
 * code will be written.
 */

typedef struct {
    char name[50];
    int param_count; // Quantos parâmetros a função recebe
    int is_non_leaf; // Flag: 1 se ela chama outras funções, 0 caso contrário
} FunctionInfo;

typedef enum {
    SYMBOL_PARAM,
    SYMBOL_VAR
} SymbolType;

// Estrutura para rastrear a localização de um símbolo na pilha.
typedef struct {
    char name[20];
    int offset;       // Deslocamento a partir do Frame Pointer ($fp)
    SymbolType type;
    int param_index;  // Se for parâmetro, qual a sua ordem (0 para $a0, 1 para $a1, etc.)
} SymbolLocation;

// Estrutura para guardar informações sobre o layout da pilha da função.
typedef struct {
    int stack_size;
    int param_count;
} FunctionLayout;


static void pre_process_and_analyze(const Quad* instructions, int instruction_count);
static FunctionInfo* find_function_info(const char* name);

static void initialize_context();
static void pre_process_data_layout(const Quad* instructions, int instruction_count);
static void get_gpr_name(const char* ir_reg, char* gpr_buffer);
static void translate_instruction_simple(const Quad* instr, FILE* out, const Quad* instructions, int current_index, int instruction_count);


 void generate_assembly(const Quad* instructions, FILE* output_file, int instruction_count);
static void initialize_function_context();
static int find_or_create_symbol_offset(const char* name);
static void get_temp_register_name(const char* ir_reg, char* mips_reg_buffer);

// --- Protótipos de Funções Auxiliares Privadas ---
static SymbolLocation* find_symbol(const char* name);
static SymbolLocation* create_symbol(const char* name, SymbolType type);
static void get_temp_register_name(const char* ir_reg, char* mips_reg_buffer);
static FunctionLayout pre_process_function_stack(const Quad* instructions, int start_index, int instruction_count);
static void translate_instruction(const Quad* instr, FILE* out, const char* func_end_label, int total_stack_size);

#endif
