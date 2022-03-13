#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
 
#include <sys/types.h>
#include <sys/wait.h>
 
static char* args[512];
pid_t pid;
int command_pipe[2];
 
#define READ  0
#define WRITE 1

static void split(char* cmd);
static int command(int input, int first, int last);
static char* skipwhite(char* s);
static int run(char* cmd, int input, int first, int last);
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
 
		int input = 0;
		int first = 1;
		char* cmd = line;
		char* next = strchr(cmd, '|'); 
		char* next_and = strchr(cmd,'&');
 
		while (next != NULL) {
			*next = '\0';
			input = run(cmd, input, first, 0);
 
			cmd = next + 1;
			next = strchr(cmd, '|'); 
			first = 0;
		}
		while (next_and != NULL) {
			*next = '\0';
			input = run(cmd, input, first, 1);
 
			cmd = next + 1;
			next = strchr(cmd, '&'); 
			first = 0;
		}
		input = run(cmd, input, first, 1);
		cleanup(n);
		n = 0;
	}
	return 0;
}
 
static int run(char* cmd, int input, int first, int last)
{
	split(cmd);
	if (args[0] != NULL) {
		if (strcmp(args[0], "exit") == 0) 
			exit(0);
		n += 1;
		return command(input, first, last);
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
static int command(int input, int first, int last)
{
	int pipettes[2];
 
	pipe( pipettes );	
	pid = fork();
 
	if (pid == 0) {
		if (first == 1 && last == 0 && input == 0) {
			dup2( pipettes[WRITE], STDOUT_FILENO );
		} else if (first == 0 && last == 0 && input != 0) {
			dup2(input, STDIN_FILENO);
			dup2(pipettes[WRITE], STDOUT_FILENO);
		} else {
			dup2( input, STDIN_FILENO );
		}
 
		if (execvp( args[0], args) == -1)
			_exit(EXIT_FAILURE); 
	}
 
	if (input != 0) 
		close(input);
	close(pipettes[WRITE]);
	if  (last == 1)
		close(pipettes[READ]);
	return pipettes[READ];
}

static void cleanup(int n)
{
	for (int i = 0; i < n; ++i) 
		wait(NULL); 
}
