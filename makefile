preassembly: libs.h preassembly.c utilities.c
	gcc -ansi -g -Wall -pedantic libs.h preassembly.c utilities.c -o preassembly