preassembly: preassembly.c utilities.c a.c
	gcc -ansi -g -Wall -pedantic preassembly.c utilities.c a.c -o preassembly