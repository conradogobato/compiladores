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
C_SRC = globals.c main.c

# Arquivos de objetos
OBJ = globals.o main.o cminus.tab.o lex.yy.o

# Regras padrão
all: $(EXEC)

# Como gerar o executável
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) -lfl

# Como gerar o arquivo do Flex
lex.yy.c: $(LEX_SRC)
	$(FLEX) $(LEX_SRC)

# Como gerar o arquivo do Bison
cminus.tab.c cminus.tab.h: $(YACC_SRC)
	$(BISON) -d $(YACC_SRC)

# Compilando os arquivos C, mas garantindo que o Bison seja rodado antes de qualquer compilação que dependa de cminus.tab.h
globals.o: globals.c cminus.tab.h globals.h
	$(CC) $(CFLAGS) -c globals.c

main.o: main.c globals.h
	$(CC) $(CFLAGS) -c main.c

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
