assembler: preassembler.c utilities.c assembler.c
	gcc -ansi -g -Wall -pedantic preassembler.c utilities.c assembler.c -o assembler
