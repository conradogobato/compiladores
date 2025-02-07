#include "globals.h"

int lineno = 1;    // ou outro valor inicial, conforme necessário
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
    printf("\nExecuting parser...\n");
    TreeNode * syntaxTree;
    printf("\nOpening file...\n");
    source = fopen("entrada.txt", "r");
    listing = stdout;
    printf("\nCreating Tree...\n");
    syntaxTree = parse(); 
    printf("\nPrinting Tree...\n");
    printTree(syntaxTree);
    printf("\nSuccess\n");
}