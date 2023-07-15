assembler: preassembler.c utilities.c assembler.c main.c
	gcc -ansi -g -Wall -pedantic preassembler.c utilities.c assembler.c main.c -o assembler
