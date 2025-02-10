#include "globals.h"
#include "analyze.h"
#include "symtab.h"

int lineno = 0;    // ou outro valor inicial, conforme necessário
int indentno = 0;  // Iniciar a indentação

FILE * source;
FILE * listing;
FILE * codeinter;
FILE * acode;
FILE * bcode;

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

    printf("\nPrinting Tree...\n");
    printTree(syntaxTree);
    printf("\nSyntax Tree printed successfully\n");
    
    printf("\nBuilding symbol table...\n");
    printf("\nPrinting symbol table...\n");
    buildSymtab(syntaxTree);
    printf("\nSuccess\n");

}