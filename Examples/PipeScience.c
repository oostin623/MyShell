/* Have a parent create a child (RightMost) which creates a 
child (M) which creates a child (LeftMost) , all piped , and 
ultimately implements the command 

	ls -l  | Upper | Lower
where those are local programs that play with character streams

Each process will have  a left and right pipe, fdl and fdr,  
(if you know what I mean) but only M uses them both. 

*/
#include <wait.h>
#include "apue.h"  // for error routines

int main ( )
{ char buffer[80];pid_t pid; 
   int Status;
  int fdl[2];int fdr[2]; // Pipe File Descriptors 
			 // 0 for reading, 1 for writing ? 
	write(STDOUT_FILENO, "Top Level\n",10);
	
	pid = fork();
      	if (pid == 0 ) // first child named "RM" for Rightmost
	{    write(1,"RM\n",3);
             pipe(fdl); 
             pid = fork(); 

	  if (pid >0 ) //RM is new parent 
		       // RM's child is "M" for Middle
		       // And  M and RM share a pipe 
 		       // Here we are still in RM 
	    { close(fdl[1]); 
			//  RM does not write to "left"
	      fdl[0]= dup2(fdl[0],STDIN_FILENO);
			// But reads from left as stdin  
 	      write (1, "Ready to Exec\n",13);
		if (((execl("./Lower","Lower",NULL))!=0)) 
			write(1,"Lower fail\n",15);
			// and transmogrifies 
             } // exit RM 

	   if (pid == 0) 
		// M's world -- first make fdl <== fdr N.B. ****
	   {	write (1,"M\n",2);
	     fdr[1]=fdl[1]; // making 
		             // M's right <==> RM's left
              fdr[1]=dup2(fdr[1],STDOUT_FILENO);
			     // and M's stdout is the pipe out 
			fdr[0]=fdl[0];close(fdr[0]);			     
			    // for symmetry	
		pipe(fdl); // Science Q ? We can pipe and repipe 
 		   // fdl because they are just integers ? 
			
	      pid = fork(); // M hatches LM (leftmost)
	
              if (pid >  0)  // Still M's world
		  {close(fdl[1]); 
			    // No write on left
			   // but read from pipe as from stdin
			fdl[0]= dup2(fdl[0],STDIN_FILENO);//MIDDLE 
                      // and transmogrify
			write (1, "Ready to Exec in M\n",19);
                     if ((execl("Upper","Upper",(char *)0)!=0)) 
                                write(1,"Upper fail\n",12);
		
                  } // exit M
/* UNTIL HERE FOR MIDDLE PROCESS */

	      if (pid == 0)   // LM's world 
			// LM has fdl and fdr
		        // and M has messed with fdl  
		        // so for symmetry ....
	          {  write(1,"LM\n",3);
		     fdr[0] = fdl[0];
		     fdr[1] = fdl[1]; close(fdr[0]);
		     fdr[1] = dup2(fdr[1],STDOUT_FILENO);
			     // and LM has no tricks on stdin because
			    // LM will not fork and no redirection '>'
                     if ((execlp("ls","./ls","-l", (char *) 0)!=0)) 
				write(1,"ls fail\n",8);
			 // transmogrify 
		  } // exit LM
            
	}  // which was within M from up there at ****

} // end of the show ?
	waitpid(pid,&Status,0);   
	write(1,"Top Level Again\n",16);
} // end of the show ? 

