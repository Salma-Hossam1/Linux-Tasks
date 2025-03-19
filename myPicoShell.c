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
	//no else in the code as i add continue or exit in each case
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

	// built-in exit
        if (strcmp(buf, "exit") == 0) {
            free(buf);
            exit(0);
        }
	// Built-in pwd
	if (strcmp (buf , "pwd") == 0)
	{
	   char *cwd = (char *)malloc(Max_Size);
	   if(cwd == NULL)
           {
              printf("Error in memory allocation\n");
	      free(buf);
              continue;
           }
           else
           {
                if(getcwd(cwd , Max_Size) == NULL)
                {
                     perror("Error in getting working directory\n");
                     free(buf);
		     free(cwd);
                     continue;
                }
                else
                {
                     int char_written = write(1,cwd ,strlen(cwd));
                     if(char_written < 0)
                     {
                         perror("Error : failed to write to terminal\n");
                         free(buf);
                         continue;
		     }
                      printf("\n");
                }
          }
        free(cwd);
	free(buf);
	continue;
	}

	//start to tokenize if no exit or pwd 
	  char *newargv [Max_Size];
	  int count =0;
	  char *token= strtok(buf, " ");
	
	  // parsing the line into tokens
	  while(token != NULL)
	    {
		newargv [count] = token;
	   	count ++;
		token = strtok(NULL," ");
	    }	
	  newargv[count] = NULL;
	  free(buf);
	  //Built-in cd
	  if (strcmp(newargv[0],"cd")==0)
	  {

	    if (chdir(newargv[1]) == 0) {
       		 continue;
   	    } else {
       
       		 printf("Failed to change directory\n");
   	    }
	  }
         //Built-in echo
          if (strcmp(newargv[0],"echo")==0)
          {
            for(int i=1 ; newargv[i] != NULL ; i++)
       	    {
               
               int written_char = write (1,newargv[i],strlen(newargv[i]));
               if(written_char < 0)
               {
                   printf("Error : failed to write %s to terminal\n",newargv[i]);
                   continue;
               }
	      write(1, " ", 1);

         //   if (i < (sizeof(newargv)/sizeof(newargv[0]) - 1))
           // {
             //  write(1, " ", 1);
     //       }
           write(1 ,"\n",1);
        continue;

        }
   //     write(1 ,"\n",1);
     //   continue;
        }
       // other commands
         pid_t ret_pid = fork();
        if (ret_pid < 0) {
            perror("fork() failed");
            continue;
        } else if (ret_pid == 0) {
            // Child process
	    execvp(newargv[0], newargv);

            // If execvp returns, it must have failed
            perror("execvp() failed");
            exit(-1);
        } else {
            // Parent process
            int status;
            wait(&status);
        }
  
	}

    }
   
