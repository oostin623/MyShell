#include <stdio.h>

int main() {

	char* cmdLine = NULL;
	char* exit = "exit";
	int Nchars;
	size_t size;
	
	printf("\n\n\nshell started. enter your command(s)\n");
	
	do {
		printf("|");
		Nchars = getline(&cmdLine, &size, stdin);
		trim off the newline
		if(cmdLine[Nchars - 1] =='\n') {
			cmdLine[Nchars - 1] = '\0';
		}
		if (Nchars == -1) {
			printf("error reading the command line.\n");
		} else {
			printf(" > %s\n",cmdLine);
		}
	} while(strcmp(cmdLine,exit) != 0);
	
	return 0;
}
