# Definindo o compilador
CC = gcc
CFLAGS = -Wall -g
FLEX = flex
BISON = bison

# Nome do executável final
EXEC = compilador

# Arquivos fontes
LEX_SRC = cminus.l
YACC_SRC = cminus.y
C_SRC = globals.c main.c symtab.c analyze.c

# Arquivos de objetos
OBJ = globals.o main.o symtab.o analyze.o cminus.tab.o lex.yy.o

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

main.o: main.c globals.h analyze.h symtab.h
	$(CC) $(CFLAGS) -c main.c

symtab.o: symtab.c symtab.h globals.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c analyze.h symtab.h globals.h
	$(CC) $(CFLAGS) -c analyze.c

cminus.tab.o: cminus.tab.c cminus.tab.h
	$(CC) $(CFLAGS) -c cminus.tab.c

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

# Limpeza dos arquivos gerados
clean:
	rm -f $(OBJ) $(EXEC) lex.yy.c cminus.tab.c cminus.tab.h

# Para rodar o compilador com o arquivo de entrada
run: $(EXEC)
	./$(EXEC) entrada.txt
