preassembly: libs.h preassembly.c utilities.c
	gcc -ansi -Wall -pedantic libs.h preassembly.c utilities.c -o preassembly