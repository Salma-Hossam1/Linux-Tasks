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
        // Print the prompt
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

        // Read user input
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

        // Skip empty input
        if (strlen(buf) == 0) {
            free(buf);
            continue;
        }

        // Built-in exit
        if (strcmp(buf, "exit") == 0) {
            free(buf);
            exit(0);
        }

        // Built-in pwd
        if (strcmp(buf, "pwd") == 0) {
            char *cwd = (char *)malloc(Max_Size);
            if (cwd == NULL) {
                printf("Error in memory allocation\n");
                free(buf);
                continue;
            }

            if (getcwd(cwd, Max_Size) == NULL) {
                perror("Error in getting working directory\n");
                free(cwd);
                free(buf);
                continue;
            }

            printf("%s\n", cwd);
            free(cwd);
            free(buf);
            continue;
        }

        // Tokenize the input
        char *newargv[Max_Size];
        int count = 0;
        char *token = strtok(buf, " ");

        while (token != NULL) {
            newargv[count] = token;
            count++;
            token = strtok(NULL, " ");
        }
        newargv[count] = NULL; // argv also ends with NULL , so we add NULL to the end

        // Built-in cd
        if (strcmp(newargv[0], "cd") == 0) {
            const char *dir = newargv[1] ;
            if (chdir(dir) != 0) {
                perror("Failed to change directory");
            }
            free(buf);
            continue;
        }

        // Built-in echo
        if (strcmp(newargv[0], "echo") == 0) {
            for (int i = 1; newargv[i] != NULL; i++) {
                printf("%s ", newargv[i]);
            }
            printf("\n");
            free(buf);
            continue;
        }

        // Execute external commands
        pid_t ret_pid = fork();
        if (ret_pid < 0) {
            perror("fork() failed");
            free(buf);
            continue;
        } else if (ret_pid == 0) {
            // Child process
            execvp(newargv[0], newargv);

            // If execvp returns, it must have failed
            perror("execvp() failed");
            free(buf);
            exit(-1);
        } else {
            // Parent process
            int status;
            wait(&status);
        }

        free(buf);
    }
    return 0;
}
