
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
SRC = main.c asdr.c tabsimb.c gerador.c
OBJ = analex.o hash.o
BIN = lpdc

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC) $(OBJ)

clean:
	rm -f $(BIN)