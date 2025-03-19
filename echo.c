#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main (int argc , char *argv[])
{
	for(int i =1 ; i < argc ; i++)
	{
		if(argv[i][0] == '$')
		{

		}
		else
		{
			int written_char = write (1,argv[i],strlen(argv[i]));
			if(written_char < 0)
			{
				printf("Error : failed to write %s to terminal\n",argv[i]);
				continue;
			}
		}
		if (i < argc - 1)
	       	{
        	    write(1, " ", 1);
       		 }
		
	}
	write(1 ,"\n",1);

	return 0;
}

