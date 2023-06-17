# This is a simple Makefile for a C project

# Name of your executable
PROG = preassembly

# Compiler to use
CC = gcc

# Flags to pass to the compiler -g adds debug symbols
CFLAGS = -g -Wall -pedantic -ansi

# Source files to compile
SRC = preassembly.c utilities.c

all: $(PROG)

$(PROG): $(SRC)
	$(CC) $(CFLAGS) -o $(PROG) $(SRC)

clean:
	rm -f $(PROG)
