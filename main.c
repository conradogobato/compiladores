#include "globals.h"
#include "analyze.h"
#include "symtab.h"
#include "cgen.h"
#include "assembler.h"
#include "binary.h" // Incluir o novo header

int lineno = 0;
int indentno = 0;

FILE * source;
FILE * listing;
// Removidos ponteiros de arquivo não utilizados para limpeza
// FILE * codeinter;
// FILE * acode;
// FILE * bcode;
// FILE * teste;
// FILE * assembly;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;


int main()
{
    // int yydebug = 1; // Descomente se precisar de debug do parser
    TreeNode * syntaxTree;
    const char* input_filename = "entrada.txt";
    const char* assembly_filename = "output.s"; // Nome mais descritivo
    const char* binary_filename = "a.out";      // Nome padrão para executável binário

    printf("\nC- COMPILER\n");
    printf("Opening source file: %s\n", input_filename);
    source = fopen(input_filename, "r");
    if (source == NULL) {
        perror("Error opening source file");
        return EXIT_FAILURE;
    }
    listing = stdout; // Saída principal para o console

    printf("\n1. Executing parser...\n");
    syntaxTree = parse();
    printf("   - Syntax tree created.\n");
    
    // As chamadas abaixo já imprimem suas próprias mensagens
    percorreArovre(syntaxTree, "global");
    printTree(syntaxTree);
    
    printf("\n2. Building symbol table...\n");
    buildSymtab(syntaxTree);
    printf("   - Symbol table built successfully.\n");

    printf("\n3. Generating intermediate code...\n");
    cGen(syntaxTree);
    printIntermediateCode();
    printf("   - Intermediate code generated.\n");

    printf("\n4. Generating assembly code to '%s'...\n", assembly_filename);
    FILE *assembly_file = fopen(assembly_filename, "w");
    if (assembly_file == NULL) {
        perror("Error opening assembly output file");
        fclose(source);
        return EXIT_FAILURE;
    }
    generate_assembly(codecodecode, assembly_file, emitLoc);
    fclose(assembly_file); // É crucial fechar o arquivo após a escrita
    printf("   - Assembly code generated successfully.\n");

    printf("\n5. Assembling to binary code to '%s'...\n", binary_filename);
    int result = assemble_to_binary(assembly_filename, binary_filename);
    if (result == 0) {
        printf("   - Binary code assembled successfully. Compilation finished.\n\n");
    } else {
        fprintf(stderr, "   - Error during binary assembly. Compilation failed.\n\n");
    }

    fclose(source);
    return 0;
}