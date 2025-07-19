#include "globals.h"
#include "analyze.h"
#include "symtab.h"
#include "cgen.h"
#include "assembler.h"

int lineno = 0;    // ou outro valor inicial, conforme necessário
int indentno = 0;  // Iniciar a indentação

FILE * source;
FILE * listing;
FILE * codeinter;
FILE * acode;
FILE * bcode;
FILE * teste;
FILE * assembly;

/* allocate and set tracing flags */
int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;


int main()
{
    int yydebug = 1;
    TreeNode * syntaxTree;

    printf("\nOpening file...\n");
    source = fopen("entrada.txt", "r");
    listing = stdout;

    printf("\nExecuting parser...\n");
    printf("\nCreating Tree...\n");
    syntaxTree = parse();

    percorreArovre(syntaxTree, "global");

    printf("\nPrinting Tree...\n");
    printTree(syntaxTree);
    printf("\nSyntax Tree printed successfully\n");
    
    printf("\nBuilding symbol table...\n");
    printf("\nPrinting symbol table...\n");
    buildSymtab(syntaxTree);
    printf("\nSuccess\n");

    cGen(syntaxTree);

    printIntermediateCode();

    const char* output_filename = "output.txt";
    FILE *output_file = fopen(output_filename, "w");

    // --- THIS IS THE CRUCIAL CHECK ---
    if (output_file == NULL) {
        // If the file couldn't be opened, print an error and exit.
        perror("Error opening output file"); // perror prints a system error message
        return EXIT_FAILURE; // or exit(1);
    }
    printf("Sexo\n");


    generate_assembly(codecodecode, output_filename);
    
}