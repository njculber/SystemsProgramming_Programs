/*
	Neil Culbertson
	Assignment 4: Shell
	Systems Programming
	Professor Scott Kuhl
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

/*
	Shell program. Runs like any other command line. Can use ls, cd and
	other commands to run programs or create files, etc.
*/
int main(){
	int pipeCount = 0;
	bool foundPipe = false;
	char *args[999];
	char *pipes[999][999];
	bool stillEnteringCommands = true;
	char cwd[1024];
   	if (!getcwd(cwd, sizeof(cwd))) perror("getcwd() error");

    char prompt[256];
    sprintf(prompt, "mtush %s> ", getcwd(NULL, 0));
	char *line = prompt;
	

	char *token = strtok(line, " ");
	do{
		printf("Starting prompt\n");
		if(sprintf(prompt, "mtush %s> ", getcwd(NULL, 0)) != (8 + (int)strlen(getcwd(NULL, 0)))){
			perror("sprintf error\n");
			return -1;
		}
		line = readline(prompt);
		if(!line){
			printf("Error readline\n");
		}

		token = strtok(line, " ");
		while(token != NULL){
			//printf("Token: %s\n", token);
			if(strcmp(token, "exit") == 0){
				free(line);
				stillEnteringCommands = false;
			}
			else if(strcmp(token, "cd") == 0){
				token = strtok(NULL, " ");
				if(!token){
					perror("Cannot enter only cd\n");
				}else{
					if(strcmp(token, "..") == 0)
						chdir("..");
					else{
						chdir(token);
					}
				}
			}
			else{
				int j = 0;
				while(token != NULL){ // loading multiple statements into one.
					if (strcmp(token, "|") == 0){
						//printf("Found PIPE\n");
						foundPipe = true;
					}
					else{
						args[j] = token;

						//printf("args[%d]: %s ", j, args[j]);
						j += 1;
						
					}
					if(foundPipe){
						printf("Pipe count: %d\n", pipeCount);
						for(int i = 0; i < j; i += 1){
							pipes[pipeCount][i] = args[i];
							//printf("pipes[%d][%d] = args[%d]; %s\n", pipeCount, i, i, args[i]);
						}
						j = 0;
						foundPipe = false;
						pipeCount += 1;
						
					}
					token = strtok(NULL, " ");
				}

				
				//printf("pipe[0][0]:%s\n", pipes[0][0]);
				//printf("pipe[0][1]:%s\n", pipes[0][1]);
				//printf("pipe[1][0]:%s\n", pipes[1][0]);

				if(pipeCount > 0){
					printf("Executing pipe\n");
					for(int i = 0; i < j; i += 1){ // fill in remaining commands
						pipes[pipeCount][i] = args[i];
						//printf("pipes[%d][%d] = args[%d]; %s\n", pipeCount, i, i, args[i]);
					}
					int fd[2];
					pipe(fd);

					int child1 = fork();
					if(child1 == 0) /* we are child 1 */
					{
						close(fd[READ_END]);
						dup2(fd[WRITE_END], STDOUT_FILENO);
						execvp(pipes[0][0], pipes[0]);
						
						//printf("execlp() failed.\n");
						exit(EXIT_FAILURE);
					}

					
					pid_t child2 = fork();
					if(child2 == 0)
					{
						
						close(fd[WRITE_END]);
						dup2(fd[READ_END], STDIN_FILENO);
						execvp(pipes[1][0], pipes[1]);

						//printf("execlp() failed.\n");
						exit(EXIT_FAILURE);
					}
					close(fd[READ_END]);
					close(fd[WRITE_END]);
					int status;
					waitpid(child2, &status, 0);
				}else{
					pid_t forkReturn = fork();
					if(forkReturn == -1)
					{
						perror("fork");
						exit(EXIT_FAILURE);
					}
					else if(forkReturn == 0)
					{
						execvp(args[0], args);
						exit(EXIT_SUCCESS);
					}
					else
					{
						int child = 0;
						pid_t waitVal = wait(&child);
						if(waitVal == -1) 
						{
							perror("wait");
							exit(EXIT_FAILURE);
						}
						else
						{
							//printf("PID %d has exited.\n", waitVal);
						}
					}
				}
			}
			memset(args, 0, sizeof(args));
			memset(pipes, 0, sizeof(args));
			pipeCount = 0;
			token = strtok(NULL, " ");
		}
	}while(stillEnteringCommands);
	return 0;
}