#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define Max_Size 1000

int main() {
    while (1) {
        char *cwd = (char *)malloc(Max_Size);
        if (cwd == NULL) {
            perror("Failed in allocating memory");
            exit(-1);
        }

        if (getcwd(cwd, Max_Size) == NULL) {
            perror("Failed in getting the prompt");
            free(cwd);
            exit(-1);
        }

        printf("%s $ ", cwd);
        free(cwd);  

        char *buf = (char *)malloc(Max_Size);
        if (buf == NULL) {
            perror("Failed in allocating memory\n");
            continue;
        }

        if (fgets(buf, Max_Size, stdin) == NULL) {
            perror("Error in fgets function\n");
            free(buf);
            continue;
        }

        // Remove the newline character at the end of the input
        buf[strcspn(buf, "\n")] = 0;
	// handle newline without command path
	if (strlen(buf) == 0){
		free(buf);
            continue;
	}


	else if (strcmp(buf, "exit") == 0) {
            free(buf);
            exit(1);
        }

        pid_t ret_pid = fork();
        if (ret_pid < 0) {
            perror("fork() failed");
            free(buf);
            continue;
        } else if (ret_pid == 0) {
            // Child process
			char *newargv[] = {buf,NULL};
			char *newenvp[] = {NULL};
            execve(buf, newargv ,newenvp);
            // If execve returns, it must have failed
            perror("execve() failed");
            exit(-1);
        } else {
            // Parent process
            int status;
            wait(&status);
        }

        free(buf);  // Free buf after use
    }

    return 0;
}
