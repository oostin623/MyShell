#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFFSIZE 1000
int main() {

	char c[BUFFSIZE];
	while(read(STDIN_FILENO, c, BUFFSIZE) > 0)
	{
		printf("if piping works this will print:\n %s\n",c);
	}
	return 0;
}
