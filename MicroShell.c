#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define Max_Size 1000

void handle_redirection(char **args);
void handle_Builtin_Functions (char **args);


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
        // if (strcmp(buf, "pwd") == 0) {
        //     char *cwd = (char *)malloc(Max_Size);
        //     if (cwd == NULL) {
        //         printf("Error in memory allocation\n");
        //         free(buf);
        //         continue;
        //     }

        //     if (getcwd(cwd, Max_Size) == NULL) {
        //         perror("Error in getting working directory\n");
        //         free(cwd);
        //         free(buf);
        //         continue;
        //     }

        //     printf("%s\n", cwd);
        //     free(cwd);
        //     free(buf);
        //     continue;
        // }

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

            // Check if the variable is local
            for (int inner_counter = 0; inner_counter < Local_Counter; inner_counter++) {
                if (strcmp(Local_VAR[inner_counter].key, newargv[i]) == 0) {
                    int len = strlen(newargv[i]) + strlen(Local_VAR[inner_counter].value) + 2;
                    char *expression_concat = malloc(len); 
                    if (!expression_concat) {
                        perror("Memory allocation failed");
                        exit(-1);
                    }
                    // We want to det that exp : KEY=VALUE
                    strcpy(expression_concat, newargv[i]); 
                    strcat(expression_concat, "="); 
                    strcat(expression_concat, Local_VAR[inner_counter].value); 
                    newargv[i] = expression_concat; 
                    break;
                }
            }
    
            if (putenv(newargv[i]) != 0) {
                perror("Error in exporting variable");
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

	//Replace $ sign
	for (int i = 1; newargv[i] != NULL; i++) {
        for(int k=0 ; newargv[i][k] != '\0' ; k++){
                    if(newargv[i][k]== '$' )
                    {
                        char *name = newargv[i]+k+1;
                        int found =0;
                        for(int j=0;j <Local_Counter ;j++)
                         {
                           if(strcmp(Local_VAR[j].key ,name) == 0)
                           {
                             strcpy(newargv[i]+k,Local_VAR[j].value);
                             found =1;
                             break;
                           }
                         }
                         if (!found) {
                            char *env_value = getenv(name);
                            if (env_value) {
                                strcpy(newargv[i]+k, env_value);
                                found =1;
                            }
                        }
                        if (!found) {
                            strcpy(newargv[i]+k, " ");
                    }
                    }
               }
            }
	
            // Check for redirection
        int has_redirection = 0;
        for (int i = 0; newargv[i] != NULL; i++) {
            if (strcmp(newargv[i], ">") == 0 || strcmp(newargv[i], "2>") == 0 || strcmp(newargv[i], "<") == 0) {
                has_redirection = 1;
                break;
            }
        }

        // Built-in functions with redirection 
        // we can't add cd -> it will change in child not in general
        if ((strcmp(newargv[0], "pwd") == 0)  || (strcmp(newargv[0], "echo") == 0) ){
            if(has_redirection)
            {

                pid_t ret_pid = fork();
        if (ret_pid < 0) {
            perror("fork() failed");
            free(buf);
            continue;
        } else if (ret_pid == 0) {
            // Child process
            handle_redirection(newargv);
            handle_Builtin_Functions (newargv);
            exit(0);
        } else {
            // Parent process
            int status;
            wait(&status);
        }
            }
            else   // No redirection
            {
                handle_Builtin_Functions(newargv);
            }
            free(buf);
             continue;
            
        }
        // Built-in cd
        if(strcmp(newargv[0], "cd") == 0){
            if(has_redirection)
            {

                pid_t ret_pid = fork();
               if (ret_pid < 0) {
                 perror("fork() failed");
                free(buf);
                continue;
                } else if (ret_pid == 0) {
                 // Child process
                 handle_redirection(newargv);
                 exit(0);
                 } else {
                     // Parent process
                     int status;
                     wait(&status);
                    //After executing redirection in child , handle cd in parent 
                    handle_Builtin_Functions (newargv);
                 }
            }
            else   // No redirection
            {
                handle_Builtin_Functions(newargv);
            }
            free(buf);
             continue;
            
        }
    
        // // Built-in cd
        // if (strcmp(newargv[0], "cd") == 0) {
        //     const char *dir = newargv[1] ;
        //     if (chdir(dir) != 0) {
        //         perror("Failed to change directory");
        //     }
        //     free(buf);
        //     continue;
        // }

   
        // Execute external commands
        pid_t ret_pid = fork();
        if (ret_pid < 0) {
            perror("fork() failed");
            free(buf);
            continue;
        } else if (ret_pid == 0) {
            // Child process
            if(has_redirection )
            {handle_redirection(newargv);}
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

void handle_redirection(char **args) {
    int fd_in = -1, fd_out = -1, fd_err = -1;
    int i, j;

    for (i = 0, j = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {  
            fd_in = open(args[i + 1], O_RDONLY);
            if (fd_in < 0) {
                perror("Error opening file for stdin redirection");
                exit(-1);
            }
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
            i++; // Skip filename
        } 
        else if (strcmp(args[i], ">") == 0) {  
            fd_out = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out < 0) {
                perror("Error opening file for stdout redirection");
                exit(-1);
            }
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
            i++; 
        } 
        else if (strcmp(args[i], "2>") == 0) {  
            fd_err = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_err < 0) {
                perror("Error opening file for stderr redirection");
                exit(-1);
            }
            dup2(fd_err, STDERR_FILENO);
            close(fd_err);
            i++; 
        } 
        else { 
            /*// type of shifting -> we want to delete arguments related to redirection (symbols and files) , if we replace them with null , it will affect that line
            //   for (int i = 0; newargv[i] != NULL; i++)
            // so trace the function */ 
            args[j++] = args[i];  

        }
    }
    args[j] = NULL;  // Terminate argument list
}

void handle_Builtin_Functions (char **args)
{
    //Built-in pwd
    if (strcmp(args[0], "pwd") == 0) {
        char *cwd = (char *)malloc(Max_Size);
        if (cwd == NULL) {
            perror("Error in memory allocation\n");
            //exit(-1); if we add exit , when we use this function without fork -> it will exit my shell
        }
        else {
            if (getcwd(cwd, Max_Size) == NULL) {
                perror("Error in getting working directory\n");
                free(cwd);
                //exit(-1);
            }
            else{
                printf("%s\n", cwd);
            }
       }
     }

    // Built-in cd
    else if (strcmp(args[0], "cd") == 0) {
        const char *dir = args[1] ;
        if (chdir(dir) != 0) {
            perror("Failed to change directory");
        }
        //free(buf);
    }

    // Built-in echo
    else if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; args[i] != NULL; i++) {
         
        printf("%s ", args[i]);

            }
        printf("\n");
        //free(buf);
    }
}