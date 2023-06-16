program: preassembly.c
	gcc -ansi -Wall -pedantic -g preassembly.c -o compiler
program.o: preassembly.c preassembly.h
	gcc -c -ansi -Wall -pedantic -g preassembly.c -o preassembly.o