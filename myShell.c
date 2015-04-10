#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/* 
*  MyShell ... 2240 Assignment #3
*
*  SPEICIFICATIONS
*
* -process unspecified number of pipes
* -handle redirection ('>' and '<')
* -handle built in cmds cd, kill, exit
* -handle backgrounding '&'
*/

//TODO: 
//remove the idiotic triple pointers
//test redirection and commands like 'grep' 
//Implement backgrounding '&'

//BUGS:
//prints weird character instead of command promt after piping
//

//++++++++++++++++PROTOTYPES

void do_cmd();
void do_pipe(int nextToken);
void do_redirect(int tokenIndex);
int makeargv(char * s, char * delimiters, char *** argvp);
//++++++++++++++++GLOBAL VARS

char*** argv;
int argc; 

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++___MAIN___+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
*	Controller for the thread, contains a loop to wait for a cmd then process it.
*/
int main() 
{
	argv = malloc(sizeof(char*));
	char* cmdLine = NULL;
	char* exit = "exit";
	char* delimiter = "|";
	int Nchars;
	size_t size;
	int i;
	char* dir;
	size_t t = 0;

	char** temp;
	int tempC;

	printf("\nshell started. enter your command(s)\n");
	
	do {
		getcwd(dir,t);
		printf("%s ### ",getcwd(dir,t));
		Nchars = getline(&cmdLine, &size, stdin);
		//trim off the newline
		if(cmdLine[Nchars - 1] =='\n') {
			cmdLine[Nchars - 1] = '\0';
		}
		if (Nchars == -1) {
			printf("error reading the command line.\n");
		} else {
			argc = makeargv(cmdLine,delimiter,argv);
			if (argc > 0 && (strcmp(cmdLine,exit) != 0)) {	

				//check for change directory
				tempC = makeargv(*((*argv))," ",&temp);
				if(strcmp(temp[0],"cd") == 0) {
					if(chdir(temp[1]) < 0) {
						printf("error: directory %s not found\n",temp[1]);
					}
				} else {
				do_cmd();getcwd(dir,t);
				}
			}
		}
	} while(strcmp(cmdLine,exit) != 0);
	
	return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++___DO_CMD___+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*
* do_cmd allows the shell to proccess a cmd without terminating itself. 
*
* Forks:
* 	parent - waits for child to finish before displaying pompt (unless & at end of cmd) 
* 		child - processes the cmd line using do_pipe
*/
void do_cmd()
{

	pid_t pd = fork();

	//parent
	if(pd != 0) {								
		char* last = *((*argv) + argc-1);
		int lastChar = strlen(last) -1;
	//	if(last[lastChar] != '&'){
		waitpid(pd, NULL, 0);         //waits for the initial child to finish executing before promting again
		printf("wait successful\n");
	//	}

			//child
			} else {					
				do_pipe(0);   //starts recursive child generation
			}	
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++___DO_PIPE___+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/*
* do_pipe recursivly processess the command line, creating a child for each token in argv[] 
* an additional process beyond the 1st child is created for each pipe
* a pipe is created before the fork, then the parnet always closes reading while the child closes writing
* 3 cases: 1st pipe, middle pipe, last pipe
* 1st pipe will always execute first and have 1 pipe, closing its reading end.
* middles have 2 pipes, one to read from the parent, then one to write to their child. must read from the parent before writing to child. first closes parents read, then closes childs write
* last pipe will always execute last, has 1 pipe, closes the writing end.
*/
void do_pipe(int nextToken)
{
	pid_t pd;
	int fd[2];

	if(nextToken < (argc-1))       //at least 1 pipe left to do:
	{
		pipe(fd); 			        //open the pipe                  
		pd = fork();

		//parent	
		if(pd != 0) {                   
			close(fd[0]);           //closes reading end of pipe   
			dup2(fd[1],1);          //redirects the parent's output to the pipe     
			do_redirect(nextToken); //executes the parent cmd


				//child
				} else {          
					close(fd[1]);           //closes writing end of pipe in child
					dup2(fd[0],0);          //redirects the pipe to child's input
					do_pipe(nextToken+1); //child spawns a child to continue with piping                                  			                        
				} 
	}
	do_redirect(nextToken); //final cmd is executed (child that never becoems a parent)
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++___DO_REDIRECT___++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
/*
* handles the redirection logic from the command line (if applicable)
* handles execution of processes using execvp
*/
void do_redirect(int tokenIndex)
{	
	char*** tokens;
	tokens = malloc(sizeof(char*));
	char* rSymbol;
	char* cmd;
	cmd = *((*argv) + tokenIndex);
	int tokenCount;
	tokenCount = makeargv(cmd, " ", tokens);
	rSymbol = *((*tokens+1));
	*((*tokens)+argc) = NULL;      //add a null to the end of the cmd array so execvp works



	if(rSymbol != NULL){ //possibility of redirection
		if(strcmp(rSymbol,"<") == 0) {
			//open file named redirArg[0]
			//set input of current process to that file
			//exec into  redirArgs[2]
		} else if (strcmp(rSymbol,">") == 0) {
			//open file named redirArgs[2]
			//set output of current process to that file
			//exec into redirArgs[0]
		}
		}else { //no redirecting, safe to just exec
			if(execvp(**tokens, *tokens) == -1) {
				printf("execvp error! could not execute %s\n",cmd);
			}
	}
}
//+++++++++++++++++++++++++++++++++++++++++++___BELOW CODE PROVIDED COURTESY OF DR. TRENARY___++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// makeargv.c 
// takes a null terminated string s, and a string of delimiters and returns 
// an array of char * pointers, leaving argvp pointing to that array.
// each of the array locations points to a null terminated "token" found in s
// defined by the delimiter array. The number of tokens found is returned by makeargv
/*
 * Make argv array (*argvp) for tokens in s which are separated by
 * delimiters.  Return -1 on error or the number of tokens otherwise.
 */
int makeargv(char * s, char * delimiters, char *** argvp)
{
	char * t;
	char * snew;
	int numtokens;
	int i;
	
	/* snew is real start of string after skipping leading delimiters */
	snew = s + strspn(s, delimiters); //very clever
	//printf("s = %s\n", s);
	//printf("delimiters = %s\n", delimiters);
	//printf("strspn = %d\n", (int) strspn(s, delimiters));
	//printf("snew = %s\n", snew);
	
	/* create space for a copy of snew in t */	
	if ((t = calloc(strlen(snew) + 1, sizeof(char))) == NULL) //only true if no more memory
	{
		*argvp = NULL;
		numtokens = -1;
	} 
	else 
	{                     /* count the number of tokens in snew */
		strcpy(t, snew);
		
		if (strtok(t, delimiters) == NULL)
			numtokens = 0;
		else
			for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++);  // THE work is in the For Construct
			//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			//start at 1 and move to the next,
			//	if its null stop
			//	else increment and move to the next
			
		/* create an argument array to contain ptrs to tokens */
		if ((*argvp = calloc(numtokens + 1, sizeof(char *))) == NULL) 
		   //^Notice the dereferencing		//Again only true if 
							//no more memory
		{
			free(t);
			numtokens = -1;
		}
		else 
		{            /* insert pointers to tokens into the array */
			if (numtokens > 0) 
			{
				strcpy(t, snew);
				**argvp = strtok(t, delimiters); //set the first token
				
				for (i = 1; i < numtokens + 1; i++)
				{
					*((*argvp) + i) = strtok(NULL, delimiters);
					//^^^^^^^^^^^^^^^^ set the other tokens
				}
			}
			else 
			{
				**argvp = NULL;
				free(t);
			}
		}
	}
	
	return numtokens;
}
