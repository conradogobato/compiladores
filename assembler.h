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
void translate_instruction(const Quad* instr, FILE* out);

void generate_assembly(const Quad* instructions, FILE* output_file);

#endif // ASSEMBLER_H
