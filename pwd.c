#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#define Max_Size 2000
int main()
{
	char *buf = (char*)malloc (Max_Size);
	if(buf == NULL)
	{
		printf("Error in memory allocation\n");
		exit(-1);
	}
	else
	{
		if(getcwd(buf , Max_Size) == NULL)
		{
			perror("Error in getting working directory\n");
			free(buf);
			exit(-1);
		}
		else
		{
			int char_written = write(1,buf ,strlen(buf));
			if(char_written < 0)
			{
				perror("Error : failed to write to terminal\n");
				free(buf);
                                exit(-1);
			}
			printf("\n");
		}
	}
	free(buf);

	return 0;
}
