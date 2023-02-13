#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define WRITE_END 1     // pipe write end
#define READ_END 0      // pipe read end

/*	shell_execute - Function to execute the command with specified command content and command arguments. 
*	@args	String array with command and its arguments. 
*	@argc	Total number of strings including the command, its arguments and the NULL in the end
*/
int shell_execute(char ** args, int argc){
	
	int child_pid, wait_return, status;

	/* Execute built-in commands */
	/* exit */
	if(strcmp(args[0], "exit") == 0 ){
		return -1; 
	}
	/* TODO: cd */
	if(strcmp(args[0], "cd")== 0){
		if(chdir(args[1])<0){
			printf("cd: %s: No such file or error \n ", args[1]);
		}
		return 0;
	}
	

	/* Non-built-in commands. These commands should be executed in a child process so the parent process can continue to invoke other commands */	
	/* One command without pipe */
	if((child_pid = fork()) < 0){
		printf("fork() error \n");
	}
	else if(child_pid == 0 ){
		/* TODO: execute the command and check if the command is correctly executed */
		if(execvp(args[0],args)<0){
			printf("execute errror\n");
			exit(-1);
		}
	}
	
	/* TODOs: one pipe and two pipes */
	int pin= -1;

for (int i = 0; i < argc; i++) {
	if (strcmp(args[i], "|") == 0) {
		pin = i;
		break;
	}
}
if (pin != -1) {
	int fd[2];
	if (pipe(fd) < 0) {
		printf("pipe error \n");
		exit(-1);
	}
	int child_pid2;
	if ((child_pid2 = fork()) < 0) {
		printf("fork() error \n");
		exit(-1);
	}
	else if (child_pid2 == 0) {
		close(fd[WRITE_END]);
		
		if (dup2(fd[READ_END], STDIN_FILENO) < 0) {
			printf("duplicate pipe error \n");
			exit(-1);
		}
		
		close(fd[READ_END]);

		if (execvp(args[pin+1], args+pin+1)< 0) {
			printf("execute error \n");
			exit(-1);
		}
	}

	else {
		close(fd[READ_END]);
		
		if (dup2(fd[WRITE_END], STDOUT_FILENO) < 0) {
			printf("duplicate pipe error \n");
			exit(-1);
		}

		close(fd[WRITE_END]);

		if (execvp(args[0], args) < 0) {
			printf("execute error \n");
		}
	}
		
}

	/* wait for child process to terminate */
	while((wait_return = wait(&status)) > 0);
			
	return 0;

}


