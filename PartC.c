#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
static char* args[512];
pid_t pid;
 
#define READ  0
#define WRITE 1

static void split(char* cmd);
static int command(int input, int output);
static char* skipwhite(char* s);
static int run(char* cmd, int input, int output);
static void cleanup(int n);
static char line[1024];
static int n = 0; 
 
int main()
{
	while (1) {
		
		printf("Abhay_Singh_Patel@12040020_$ ");
		fflush(NULL);
 
		
		if (!fgets(line, 1024, stdin)) 
			return 0;

		char *inputFile = NULL;
		char *outputFile = NULL;
		char* cmd = line;
 		char* outputRedir = strchr(cmd, '>'); 
 		char* appendRedir = strstr(cmd, ">>"); 
 		int input,output;
 		if(appendRedir!=NULL){
 			*appendRedir = '\0';
 			outputFile = strtok(appendRedir + 2, " \t\n");
 			output = open(outputFile, O_CREAT | O_RDWR | O_APPEND,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH );
 		}
 		else if(appendRedir==NULL && outputRedir!=NULL){
 			*outputRedir = '\0';
 			outputFile = strtok(outputRedir + 1, " \t\n");
 			output = open(outputFile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
 		}
 		else
 			output = 0;

		
		input = run(cmd, input,output);
		if (input!=0){
			close(input);
		}
		if (output!=0){
			close(output);
		}
		cleanup(n);
		n = 0;
	}
	return 0;
}
static int run(char* cmd, int input, int output)
{
	split(cmd);
	if (args[0] != NULL) {
		if (strcmp(args[0], "exit") == 0) 
			exit(0);
		n += 1;
		return command(input, output);
	}
	return 0;
}
static char* skipwhite(char* s)
{
	while (isspace(*s)) ++s;
	return s;
}
static void split(char* cmd)
{
	cmd = skipwhite(cmd);
	char* next = strchr(cmd, ' ');
	int i = 0;
 
	while(next != NULL) {
		next[0] = '\0';
		args[i] = cmd;
		++i;
		cmd = skipwhite(next + 1);
		next = strchr(cmd, ' ');
	}
 
	if (cmd[0] != '\0') {
		args[i] = cmd;
		next = strchr(cmd, '\n');
		next[0] = '\0';
		++i; 
	}
 
	args[i] = NULL;
}
static int command(int input, int output)
{	
	pid = fork();
 
	if (pid == 0) {
		if (output==0) {
			dup2( input, STDIN_FILENO ); 
		}else {
			dup2( input, 0);
			dup2(output, 1);
		}
		if (execvp( args[0], args) == -1)
			_exit(EXIT_FAILURE);
	}
 
	if (input != 0) 
		close(input);
 
	return 0;
}
static void cleanup(int n)
{
	int i;
	for (i = 0; i < n; ++i) 
		wait(NULL); 
}
