#Nome do arquivo fonte
SOURCE = banker.c

#Nome do executável
EXECUTABLE = banker

#Compilador
CC = gcc

#Opções de compilação
CFLAGS = -Wall -g

#Regra padrão
all: $(EXECUTABLE)

#Construir o executável
$(EXECUTABLE): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(EXECUTABLE)

#Limpar arquivos compilados
clean:
	rm -f $(EXECUTABLE)
