#include <stdio.h> // Steven's Figure 15.14 "UpCase" 
#include "apue.h"
int main()
{  int c;

	while (( c = getchar()) != EOF)
	{	if ( islower(c))
			c = toupper(c);
		if (putchar(c) == EOF)
		   err_sys("output error in upcase\n");
		if (c == '\n') 
		   fflush(stdout);
	}
	exit(0);
}
