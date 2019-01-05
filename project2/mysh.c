#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <ctype.h>

#define MAXCMDLEN 514

const char error_message[30] = "An error has occurred\n";

struct command {
	char **str;
	char *filepath;
	int redirection;
	int background_jobs;
	int num;
};
typedef struct command COMMAND;

void printError();
int readcommand(COMMAND*);
int cmd_pwd();
void cmd_wait();
int cmd_cd(COMMAND*);

int mode;
FILE *input;
char cmdline[MAXCMDLEN + 1];

int main(int argc, char *argv[])
{
	if (argc > 2) {
		printError();
		exit(0);
	}
	if (argc == 2) {
		mode = 1;  // batch mode
		input = fopen(argv[1], "r");
		if (input == NULL) {
			printError();
			exit(0);
		}
	}
	else {
		mode = 0; // interactive mode
		input = stdin;
	}

	COMMAND cmd;
	int savefd = dup(STDOUT_FILENO);
	while (1) {
		dup2(savefd, STDOUT_FILENO);
		if (mode == 0)
			printf("mysh> ");

		if (readcommand(&cmd) == -1) {
			printError();
			continue;
		}
		
		if (cmd.num == 0) 
			continue;

		if (strcmp(cmd.str[0], "exit") == 0) {
			if (cmd.num == 1) {
				free(cmd.str);
				exit(0);
			}
			else {
				printError();
				continue;
			}
		}
		else if (strcmp(cmd.str[0], "pwd") == 0) {
			if (cmd.num != 1 || cmd_pwd(&cmd) == -1) {
				printError();
				continue;
			}
		}
		else if (strcmp(cmd.str[0], "wait") == 0) {
			if (cmd.num == 1)
				cmd_wait();
			else {
				printError();
				continue;
			}
		}
		else if (strcmp(cmd.str[0], "cd") == 0) {
			if (cmd.num != 1 && cmd.num != 2) {
				printError();
				continue;
			}
			if (cmd_cd(&cmd) == -1) {
				printError();
				continue;
			}
		}
		else {
			int rc = fork();
			if (rc < 0) {
				printError();
				exit(1);
			}
			else if (rc == 0) {
				//child process
				cmd.str[cmd.num] = NULL;
				int fp;
				if (cmd.redirection == 1) {
					savefd = dup(STDOUT_FILENO);
					fp = open(cmd.filepath, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
					if (fp == -1) {
						printError();
						exit(0);
					}
					dup2(fp, STDOUT_FILENO);
				}
				if (execvp(cmd.str[0], cmd.str) == -1) {
					printError();
					exit(0);
				}
			}
			else {
				//parent process
				if (cmd.background_jobs == 0)
					wait(NULL);
			}
		}
		free(cmd.str);
	}
	return 0;
}

void printError()
{
	write(STDERR_FILENO, error_message, strlen(error_message));
}

int readcommand(COMMAND* cmd)
{
	char cmdbuffer[MAXCMDLEN + 1];
	char c[2] = {0};

	if (fgets(cmdbuffer, MAXCMDLEN, input) != NULL) {
		if (mode == 1){
			write(STDOUT_FILENO, cmdbuffer, strlen(cmdbuffer));
			if (strchr(cmdbuffer, '\n') == NULL) {
				c[0] = '\n';
				write(STDOUT_FILENO, c, strlen(c));
			}
		}
		// the cmd len out of MAXCMDLEN
		if (!feof(input) && strchr(cmdbuffer, '\n') == NULL) {
			while ((c[0] = fgetc(input)) != '\n' && ~c[0]) {
				if (mode == 1)
					write(STDOUT_FILENO, c, strlen(c));
			}
			write(STDOUT_FILENO, c, strlen(c));
			return -1;
		}

		int len = strlen(cmdbuffer);
		int i, count_len = 0, count_cmd = 0, count_redirection = 0;
		for (i = 0; i < len; i ++) {
			if (isspace(cmdbuffer[i])) {
				if (i && isspace(cmdbuffer[i]) && (!count_len || cmdline[count_len - 1])) {
					count_cmd += 1;
					cmdline[count_len++] = '\0';
				}
			}
			else {
				if (cmdbuffer[i] == '>') {
					count_redirection += 1;
					if (count_len && cmdline[i-1])
						cmdline[count_len++] = '\0';
					cmdline[count_len++] = cmdbuffer[i];
					cmdline[count_len++] = '\0';
				}
				else if (cmdbuffer[i] == '&') {
					if (count_len && cmdline[i-1])
						cmdline[count_len++] = '\0';
					cmdline[count_len++] = cmdbuffer[i];
				}
				else 
					cmdline[count_len++] = cmdbuffer[i];
			}
		}
		if (count_redirection > 1)
			return -1;
		if (count_len > 1 && cmdline[count_len-2] == '>')
			return -1;

		cmd->num = 0;
		cmd->filepath = NULL;
		cmd->redirection = 0;
		cmd->background_jobs = 0;

		if (count_len > 1 && cmdline[count_len - 2] == '&')
		{
			cmd->background_jobs = 1;
			if ((count_len - 2) != 0)
				count_len -= 2;
		}

		cmd->str = (char**)malloc(sizeof(char*) * (count_cmd - count_redirection + 1));
		for (i = 0; i < count_len; i ++)
			if (cmdline[i] != '\0') {
				if (cmdline[i] == '>') {
						cmd->redirection = 1;
					cmd->filepath = cmdline + i + 2;
					i += 2;
				}
				else if (!i || !cmdline[i - 1])
					cmd->str[(cmd->num)++] = cmdline + i;
			}
		if ((cmd->redirection) && (cmd->filepath) < (cmd->str[(cmd->num) - 1]))
			return -1;
	}
	else
	{
		if (feof(input)) // the file input ends
			exit(0);
		else
			return -1; // the file gets fail
	}
	return 0;
}

int cmd_pwd(COMMAND *cmd)
{
	char *path = NULL;
	path = getcwd(NULL, 0);
	if (path == NULL)
		return -1;
	if (cmd->filepath) {
		int fp = open(cmd->filepath, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
		if (fp == -1)
			return -1;
		if (write(fp, path, strlen(path)) == -1)
			return -1;
		else
			write(fp, "\n", 1);
	}
	else {
		if (write(STDOUT_FILENO, path, strlen(path)) == -1)
			return -1;
		else
			write(STDOUT_FILENO, "\n", 1);
	}
	free(path);
	return 0;
}

void cmd_wait()
{
	while (wait(NULL) != -1);
	return;
}

int cmd_cd(COMMAND* cmd)
{
	if (cmd->num == 1 && chdir(getenv("HOME")) == -1)
		return -1;
	if (cmd->num == 2 && chdir(cmd->str[1]) == -1)
		return -1;
	return 0;
}