#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#define Max_Size 1024

#define RET 0

int main()
{
	while(1)
	{
		printf("Femto shell prompt -> ");
		char *buf = (char *)malloc(Max_Size);
		if(fgets(buf , Max_Size , stdin)== NULL)
		{
			perror("Failed to read the command\n");
			free(buf);
		}
		else
		{
			char command[5];
			if(strncpy(command , buf,4)!= NULL)
			{
				if(strcmp (command , "exit")==0)
				{
					write(1,"Good bye\n" , strlen("Good bye\n"));
					free(buf);
					exit(0);
				}
				else if(strcmp (command , "echo")==0)
				{
					printf("%s",buf+5);
				}
				else if(strcmp (command ,"\n") ==0)
				{
					continue;
				}
				else
				{
					printf("Invalid command\n");
				}
			}
			else
			{
				perror("Failure in strncpy\n");
			}

		}


	}
	return 0;

}  
