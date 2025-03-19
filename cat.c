#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
	//check if agrguments is not enough 
    if (argc < 2) 
    {
        printf("Usage: %s <file1> <file2> ...\n", argv[0]);
        // return error number ->-1
        exit(-1);
    }

    for (int i = 1; i < argc; i++) 
    {
        int fd_source = open(argv[i], O_RDONLY);
        if (fd_source == -1) 
        {
            printf("Error: Cannot open file '%s'\n", argv[i]);
            continue; // if i want to continue even if a file fail 
        }

        // Get the size of the file
        int size = lseek(fd_source, 0, SEEK_END);
        //After using lseek to get the file size , we should reset the file pointer to the beginning 
        lseek(fd_source, 0, SEEK_SET); 

 
        char *buf = (char *)malloc(size);
        if (buf == NULL) 
        {
            printf("Error: Memory allocation failed for file '%s'\n", argv[i]);
            close(fd_source);
            continue;
        }


        int num_read = read(fd_source, buf, size);
        // -1 is the return on error is also smaller than size 
        if (num_read != size) 
        {
            printf("Error: Failed to read file '%s'\n", argv[i]);
            free(buf);
            close(fd_source);
            continue;
        }

        
        int num_written = write(1, buf, size); //I can replace size here with num_read because they are equal from the last check
        // check error -> -1 , but here I used the variable size , as I got the size anyway
        if (num_written != size) 
        {
            printf("Error: Failed to write file '%s' to terminal\n", argv[i]);
        }

        free(buf);
        close(fd_source);
    }

    return 0;
}
