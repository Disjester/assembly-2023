# This is a simple Makefile for a C project

# Name of your executable
PROG = preassembly

# Compiler to use
CC = gcc

# Flags to pass to the compiler -g adds debug symbols
CFLAGS = -g -Wall -pedantic

# Source files to compile
SRC = preassembly.c utilities.c

# Object files to generate
OBJ = $(SRC:.c=.o)

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) $(CFLAGS) -o $(PROG) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROG) $(OBJ)
