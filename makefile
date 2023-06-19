preassembly: preassembly.c utilities.c
	gcc -ansi -g -Wall -pedantic preassembly.c utilities.c -o preassembly