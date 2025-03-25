#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

#define Max_Size 1000

typedef struct
{
	char *key ;
	char *value  ;
}LocalVAR;

int main() {
	LocalVAR Local_VAR[Max_Size];

	// initialize counter to count no of local variables with 0
	int Local_Counter =0;
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
			       
        // ~ -> replace ~ with home directory
	for(int i=1 ; newargv[i]!=NULL ; i++)
	{
          if(newargv[i][0] == '~')
	  {
             char *current_home = getenv("HOME");
	     newargv[i] = newargv[i]+1;  // delete ~ from argument
	     int size = strlen(newargv[i]);
	     strncat(current_home , newargv[i] , size);
	     int s = strlen(current_home);
	     strncpy(newargv[i],current_home,s);
	  }

	}


	//handle variables 
	if (newargv[0] && strchr(newargv[0], '=')!= NULL)
	{
		//Make sure that each token in the command is valid before updating any value
		for(int count =0 ; newargv[count]!= NULL ;count ++)
		{
			if (strchr( newargv[count], '=')== NULL)
			{
				printf("%s is not an argument\n",newargv[count]);
				exit(-1);
			}
		}
		// outer loop to handle all arguments if the user wants to set some values in same line
		for(int count =0 ; newargv[count]!= NULL ;count ++)
                {
			int inner_counter = 0;
			int name_length = ( strchr( newargv[count], '='))- newargv[count];
			int val_length = strlen(strchr( newargv[count], '=')+1);
			
			char *name = (char *)malloc(name_length + 1);
                        char *val = (char *)malloc(val_length + 1);
                        if (name == NULL|| val == NULL) {
            			perror("Memory allocation failed");
            			exit(-1);
        		}	

		       stpncpy(val, strchr( newargv[count], '=') +1,val_length) ;
		       strncpy(name ,newargv[count] ,name_length);
		       
		       name[name_length] = '\0';
		       val[val_length] ='\0';

	                 // if the key is added before then update the value 
			 for (inner_counter =0 ; inner_counter < Local_Counter; inner_counter++)
			 {
                           if (strcmp (Local_VAR[inner_counter].key ,name) == 0)
		        	{
			          Local_VAR[inner_counter].value = val;
			          free(name);
				  break;
				}
			 }
			 // ADD new variable
			  if(inner_counter == Local_Counter){
				  Local_VAR[Local_Counter].key = name ;
			          Local_VAR[Local_Counter].value = val;
			          Local_Counter ++;
			  }


                }
		continue;
	}

	//handle environment variables
	if(strcmp(newargv[0], "export") == 0){
		for(int i=1 ;newargv[i] != NULL ; i++){
		 if(putenv(newargv[i]) != 0){
		   perror("error in exporting variable");
		   }
		}
	  continue;
	}
	
	if(strcmp(newargv[0], "unset") == 0){
                for(int i=1 ;newargv[i] != NULL ; i++){
                 if(unsetenv(newargv[i]) != 0){
                   perror("error in deleting variable");
		   }

                }
          continue;
        }

	// if(strcmp(newargv[0] ,"printev")){
	//   for(int i=1 ; newargv[i] != NULL ; i++){
	//     if(getenv(newargv[i] !=NULL )){
	//          printf("%s\t",getenv(newargv[i]));
	// 		    }

	// 		    }
	//    continue;
	//   }

	// 		}
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
		    if(newargv[i][0]== '$')
		    {
			char *name = newargv[i]+1;
			int found =0;
			for(int j=0;j <Local_Counter ;j++)
			 {
		           if(strcmp(Local_VAR[j].key ,name) == 0)
			   {
			     printf("%s ", Local_VAR[j].value);
			     found =1;
			     break;
			   }
			 }
			if(!found) //search in environment variable
		            {
                           if(newargv[i] != NULL && getenv(name) !=NULL ){
                            printf("%s ",getenv(name));
                            found =1;

                            }
			    }
			    }
			    else 
			{printf("%s ", newargv[i]);}

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
