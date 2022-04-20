#include<sys/wait.h>
#include<sys/types.h>
#include<signal.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include"list.c"

void cmd_error(){
	printf("Cmd error! Please input run | list | kill | quit.\n");
}

void cmd_run(char* cmd){
	char file[20];
	int i;
	for(i=4;;i++){
		if(cmd[i] == '\0')
			break;
		file[i-4]=cmd[i];
	}
	file[i-4] = '\0';
	int pid;
    /* fork another process */
    pid = fork();
    if (pid<0) // pid都是大于等于0的
    { 
            /* error occurred */
            printf("Fork Failed!\n");
            exit(-1);
    } 
    else if (pid==0) // 子进程的返回值（eax寄存器保存）是0，所以子进程进入else if条件分支
    {
            /*  child process   */

	    char *envp[] = {NULL};
	    char *argv_send[] = {file,NULL};
	    execve(file,argv_send,envp);
    } 
    else  // 父进程的返回值（eax寄存器保存）> 0，所以父进程进入else条件分之
    { 
            /*    parent process  */
            /* parent will wait for the child to complete*/
            wait(NULL);
    }
}

void cmd_kill(char* cmd){
	int i = 5;
	int pid = 0;
	for(;;i++){
		if(cmd[i] != '\0'){ 
			pid = pid * 10 + cmd[i]-'0';    //get pid
		}else{
			break;
		}
	}
	int rs = kill(pid, SIGKILL);        //kill
	if(!rs){
		printf("kill success!\n");
	}else{
		printf("kill error!\n");
	}
}

//get cmd: run 1 | list 2 | kill 3 | quit 4
int getcmd(char* cmd){  
	int i = 0;
	int rs = 0;
	if(cmd[0] == 'r' && cmd[1] == 'u' && cmd[2] == 'n')
		rs = 1;
	else if(cmd[0] == 'l' && cmd[1] == 'i' && cmd[2] == 's' && cmd[3] == 't')
		rs = 2;
	else if(cmd[0] == 'k' && cmd[1] == 'i' && cmd[2] == 'l' && cmd[3] == 'l')
		rs = 3;
	else if(cmd[0] == 'q' && cmd[1] == 'u' && cmd[2] == 'i' && cmd[3] == 't')
		rs = 4;
	return rs;
}

int main(){
	printf("-------Your bash----------\n");
	printf("please input run | list | kill | quit.\n");
	char cmd[50];
	int c;
	while(1){
		printf("bash >> ");
		gets(cmd);
		c = getcmd(cmd);
		switch(c){
			case 0: cmd_error(); break;
			case 1: cmd_run(cmd); break;
			case 2: cmd_list(cmd); break;
			case 3: cmd_kill(cmd); break;
			case 4: return 0;
		}
	}
	
	return 0;

}
