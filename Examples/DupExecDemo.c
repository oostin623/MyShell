// This code is intended to demo small pieces of code relevant to a shell
// dup2, execv()  in particular
// In addition, the most useful makeargv() method will be used 
//
#include <stdio.h>
#include "apue.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>   // OverKill 
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
//
int makeargv(char *, char * , char ***);
int main()
{int NewFD,i,j;
char ** ExecVectors;  
char * ExecArgs[10];
	write(STDOUT_FILENO,"To Console\n",11);
// Now open a file and get a file descriptor
	mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR; // FOR OPEN AND CREATE OF FILES 
	int oflag = (O_CREAT | O_RDWR | O_TRUNC);
	NewFD = open("DummyFile",oflag,mode);
	printf("DummyFile descriptor is %u\n",NewFD);
	NewFD=dup2(NewFD,STDOUT_FILENO);
	printf("Dup File descriptor is %u\n",NewFD);
write(STDOUT_FILENO,"To Console ?\n",13);

// execlp("ls"," -l","/",NULL);

/* ExecArgs[0]="ls";
ExecArgs[1]="-l";
ExecArgs[2]="/";
ExecArgs[2]=NULL;
execvp(ExecArgs[0],&ExecArgs[0]); */

j=makeargv("ls -l / | cat DupExecDemo.c","\t \n",&ExecVectors);
for (i=0;i<j;i++) fprintf(stderr,"%s\n",ExecVectors[i]);
// ExecVectors[3] = NULL; // useless
execvp(ExecVectors[0],&ExecVectors[0]);
err_sys("exec failed\n");

}
