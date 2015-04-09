
#include <stdio.h>
#include "apue.h"        // A demo of simple pipe
#include <unistd.h>        // from child to parent
#include <sys/types.h>        // They tire of it, and exec
#include <sys/wait.h>        // into ls and upper ? ,
                // respectively
int main()
{int fd[2];  // an int array to be used by the pipe system call
  pid_t pid;  // the universal process id variable
    int j; // the universal integer
    char buf[5]; // a modest buffer
  if (pipe(fd)== -1) err_sys("pipe error");
       if((pid=fork())<0)err_sys("forked badly");

    if (pid == 0)                 // child
    {close(fd[0]); // won't be reading pipe
        write(1," before stuff",015);
     write(fd[1],"some characters",15);//some piped letters
    fd[1]=dup2(fd[1],STDOUT_FILENO); // this process' Standard Out
    write(1,"after stuff",5);
    execlp("/bin/ls", "ls","-l",(void  *)0);
	write(1,"ls failed\n",10);
                   // is now piped
            // and finally child becomes ls
    }
   
    if (pid > 0 )  // Parent
    {close(fd[1]);  // no write to pipe here   
        fd[0] = dup2(fd[0], STDIN_FILENO);
      write (STDOUT_FILENO, "parent",6); // message to stdout
// This turns parent into Upcaser
    execlp("./Upcase","Upcase",NULL);    // your code might have something like
            // execvp(argvp[place], &argvp[place]) where
            // you have modified argvp array with NULL
            // in appropriate places (like where the | is)
        }   
}

