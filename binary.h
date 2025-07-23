#ifndef BINARY_H
#define BINARY_H

#include <stdint.h> // Para usar uint32_t, uint8_t, etc.

/**
 * @brief Monta um arquivo de assembly (em sintaxe MIPS) para um arquivo binário
 * para a arquitetura customizada definida.
 *
 * Esta função implementa um montador de duas passagens.
 * Passagem 1: Encontra todos os labels e seus endereços de memória.
 * Passagem 2: Lê, traduz e converte cada instrução MIPS para seu formato
 * binário customizado de 32 bits.
 *
 * @param input_assembly_filename O caminho para o arquivo de assembly (.txt ou .s).
 * @param output_binary_filename O caminho para o arquivo binário de saída (.bin).
 * @return 0 em caso de sucesso, -1 em caso de erro.
 */

// Formatos das instruções customizadas
typedef enum {
    FORMAT_1, FORMAT_2, FORMAT_3, FORMAT_4, FORMAT_5,
    FORMAT_6, FORMAT_7, FORMAT_8, FORMAT_9, FORMAT_10,
    FORMAT_PSEUDO,
    FORMAT_UNSUPPORTED
} InstructionFormat;

typedef struct {
    char name[20];
    uint8_t opcode;
    InstructionFormat format;
} OpcodeInfo;

typedef struct {
    char name[10];
    uint8_t number;
} RegisterInfo;

typedef struct {
    char name[50];
    uint32_t address;
} LabelInfo;


int assemble_to_binary(const char* input_assembly_filename, const char* output_binary_filename);

#endif // BINARY_H