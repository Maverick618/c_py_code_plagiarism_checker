#define _GNU_SOURCE

#include <sched.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <readline/readline.h>
#include <malloc.h> 
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>


#define CMD_NUMS 1024
#define CMD_LENGH 10

#define CMD_RUN "run"
#define CMD_LIST "list"
#define CMD_KILL "kill"
#define CMD_QUIT "quit"
#define CMD_PROCESSOR_NUM "cpunum"

//how to change the color of the prompt

char cmd_buf[256][256];
int buf_pos = 0;
int cpu_num;

typedef struct process{
	struct process *next;
	char **argv;
	pid_t pid;
	bool completed;
	bool stopped;
	int status;
}process;

process *start;

void cmd_run(char *);
void cmd_list(char *);
void cmd_kill(char *);
void cmd_quit(char *);
void cmd_processor_num(char *);

pid_t procs[256] = {0,};

int main(int argc, char** argv){
	start = (process*)malloc(sizeof(process));
	cpu_num = get_nprocs_conf();
	while(true){
		char *cmd_temp_buf = readline("Super Shell % ");
		strcpy(cmd_buf[buf_pos], cmd_temp_buf);
		free(cmd_temp_buf);	
		if(strlen(cmd_temp_buf) >= 256){
			printf("Command length exceeds. Please input command less than 256 characters.\n");
			continue;
		}
		if(cmd_buf[buf_pos][0]){
			char *cur_cmd = cmd_buf[buf_pos];
			if(!strncmp(CMD_RUN, cur_cmd, 3)){
				cmd_run(cur_cmd);	
			}
			else if(!strncmp(CMD_LIST, cur_cmd, 4)){
				cmd_list(cur_cmd);	
			}
			else if(!strncmp(CMD_KILL, cur_cmd, 4)){
				cmd_kill(cur_cmd);	
			}
			else if(!strncmp(CMD_QUIT, cur_cmd, 4)){
				cmd_quit(cur_cmd);	
			}
			else if(!strncmp(CMD_PROCESSOR_NUM, cur_cmd, 6)){
				cmd_processor_num(cur_cmd);
			}
			else{
				printf("Not a valid command!\n");
			}
			buf_pos++;
			if(buf_pos == 256){
				buf_pos = 0;
			}
		}
	}
}

int isbackslash(char c){
	return (c == '/') ? 1 : 0;
}

void cmd_run(char *cmd){
	int pid = fork();
	int stat;
	if(pid == (pid_t)-1){
		printf("fork() error");
		return;
	}else if(pid == 0){
		int len = strlen(cmd);
		char path[256];
		int cur_pos;
		if(len <= 4){
			printf("Please input the program name!\n");
			exit(EXIT_SUCCESS);
		}else{
			cur_pos = 4;
			while(isbackslash(cmd[cur_pos]) || isalnum(cmd[cur_pos]) || (cmd[cur_pos] == '_')){
				cur_pos++;	
			}
			strncpy(path, &cmd[4], cur_pos-4);
			path[cur_pos-4] = '\0';
			while(isblank(cmd[cur_pos]))cur_pos++;
			if(cmd[cur_pos] != '\0'){
				if(cmd[cur_pos] == '-'){
					char processor_string[256];
					cur_pos++;
					cpu_set_t cpu_mask;
					CPU_ZERO(&cpu_mask);
					switch(cmd[cur_pos]){
						case 'j':
							cur_pos++;
							printf("The process is now running on CPU cores:");
							sprintf(processor_string, "The process is now running on CPU cores:");
							for(;cmd[cur_pos] != '\0' && isdigit(cmd[cur_pos]);cur_pos++){
								int n = (int)cmd[cur_pos] - 48;
								if(n < 0 || n > cpu_num){
									printf("Wrong CPU numbers.\n");
									exit(EXIT_FAILURE);
								}	
								CPU_SET(n, &cpu_mask);
								char temp[256];
								sprintf(temp, " %d", n);
								strcat(processor_string, temp);
							}
							strcat(processor_string, ".\n");
							printf("%s", processor_string);
							while(isblank(cmd[cur_pos]))cur_pos++;
							if(cmd[cur_pos] != '\0'){
								printf("Invalid commands!\n");
								exit(EXIT_FAILURE);
							}
							sched_setaffinity((pid_t)0, sizeof(cpu_set_t), &cpu_mask);
							break;
						case 'b':
							
							break;
					}
				}else{
					printf("The parameter is not valid!\n");
					exit(EXIT_SUCCESS);
				}
				
			}
			execl(path, &cmd[4], NULL);
			exit(EXIT_SUCCESS);
		}
	}else{
		wait(&stat);
		return;
	}
}
void cmd_list(char *cmd){
	char *pos = cmd + 5;
	int num = atoi(pos);
	char path[256] = "cat /proc/";
	strcat(path, pos);
	strcat(path, "/status");
	system(path);
	//system("ps");
	//
}
void cmd_kill(char *cmd){
	pid_t pid;
	int cur_pos = 4;
	while(isblank(cmd[cur_pos]))cur_pos++;
	pid = atoi(&cmd[cur_pos]);
	while(isblank(cmd[cur_pos]))cur_pos++;
	kill(pid, SIGTERM);
	printf("pid %d is killed.\n", pid);
}
void cmd_quit(char *cmd){
	exit(EXIT_SUCCESS);
}
void cmd_processor_num(char *cmd){
	printf("There are %d CPU cores available.\n", cpu_num);
}
