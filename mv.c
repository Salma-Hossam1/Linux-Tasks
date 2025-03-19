#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define RET 0

int main(int argc , char* argv[]) {
  
    if (argc == 3) {
     
        int fd_source = open(argv[1], O_RDONLY);
        if (fd_source == -1) {
            printf("Error opening source file");
            exit(-1);
        }


        // Get the size of the source file using lseek
        int size = lseek(fd_source, 0, SEEK_END);
        lseek(fd_source, 0, SEEK_SET);
       

        // Open the destination file in write-only mode, create if doesn't exist
        int fd_dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd_dest == -1) {
            printf("Error opening destination file\n");
            close(fd_source);
            exit(-1);
        }


        char* buf = (char*) malloc(size);
	if(buf == NULL)
	{
		printf("Error : Memory allocation failed\n");
		close(fd_source);
		close(fd_dest);
		exit(-1);
	}

       
        int char_read = read(fd_source, buf, size);
    
	 if (char_read != size)
	 {
                printf("Error: Failed to read file '%s'\n", argv[1]);
                free(buf);
                close(fd_source);
		close(fd_dest);
                exit(-1);
       
       	 }

        int char_written = write(fd_dest, buf, size);
        if (char_written != size)
       	{
            printf("Error: Failed to write to file '%s' \n", argv[2]);
	    free(buf);
            close(fd_source);
            close(fd_dest);
            exit(-1);
        }

        free(buf);
	close(fd_source);
        close(fd_dest);
	if(unlink(argv[1])==-1)
	{
		perror("Failed to remove source file\n");
	}

    }
    else if (argc < 3)
    {
        printf("Too few arguments\n");
	exit(-1);
    } else {
        printf("Too many arguments\n");
	exit(-1);
    }

    return RET;
}

