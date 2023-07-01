preassembler: preassembler.c utilities.c
	gcc -ansi -g -Wall -pedantic preassembler.c utilities.c -o preassembler
