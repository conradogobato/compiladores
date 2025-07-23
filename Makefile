# Definindo o compilador
CC = gcc
CFLAGS = -Wall -g
FLEX = flex
BISON = bison

# Nome do executável final
EXEC = compilador

# --- DEFINIÇÕES DOS ARQUIVOS FONTES ---
# Adicionadas as definições que estavam faltando
LEX_SRC = cminus.l
YACC_SRC = cminus.y
C_SRC = globals.c main.c symtab.c analyze.c cgen.c assembler.c binary.c

# Arquivos de objetos
OBJ = globals.o main.o symtab.o analyze.o cgen.o assembler.o binary.o cminus.tab.o lex.yy.o

# Regras padrão
all: $(EXEC)

# Como gerar o executável
$(EXEC): $(OBJ)
	$(CC) -g $(OBJ) -o $(EXEC) -lfl

# Como gerar o arquivo do Flex
lex.yy.c: $(LEX_SRC)
	$(FLEX) $(LEX_SRC)

# Como gerar os arquivos do Bison
cminus.tab.c cminus.tab.h: $(YACC_SRC)
	$(BISON) -d $(YACC_SRC)

# Compilando os arquivos C
globals.o: globals.c cminus.tab.h globals.h
	$(CC) $(CFLAGS) -c globals.c

# Adicionada dependência de binary.h
main.o: main.c globals.h analyze.h symtab.h cgen.h assembler.h binary.h
	$(CC) $(CFLAGS) -c main.c

symtab.o: symtab.c symtab.h globals.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c analyze.h symtab.h globals.h
	$(CC) $(CFLAGS) -c analyze.c

cgen.o: cgen.c cgen.h globals.h
	$(CC) $(CFLAGS) -c cgen.c

assembler.o: assembler.c assembler.h globals.h
	$(CC) $(CFLAGS) -c assembler.c

# Nova regra de compilação para binary.o
binary.o: binary.c binary.h
	$(CC) $(CFLAGS) -c binary.c

cminus.tab.o: cminus.tab.c cminus.tab.h
	$(CC) $(CFLAGS) -c cminus.tab.c

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

# Limpeza dos arquivos gerados
clean:
	rm -f $(OBJ) $(EXEC) lex.yy.c cminus.tab.c cminus.tab.h

# Para rodar o compilador com o arquivo de entrada
# O comando 'make run' irá primeiro construir o executável (se necessário) e depois rodá-lo
run: $(EXEC)
	./$(EXEC)