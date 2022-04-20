#include <unistd.h>
#include <sched.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include "string.h"
#include "math.h"
#include <signal.h>
#define __USE_GNU
#define _GNU_SOURCE
void runnnn(char *p[10]);   //运行程序函数声明
char ppath[30] ={0};        //存放当前路径
char hostname[30] ={0};     //存放主机名称
int isprocdir(char dir[])   //查看目录是否是纯数字（进程名）
{
	int len = strlen(dir);
	if(len==0)
		return 0;
	for(int i=0;i<len;i++)
	{
		if(dir[i]<'0'||dir[i]>'9')
			return 0;
	}
	return 1;
}
void printinfo()    //list功能实现
{
	printf("PNAME\t\t\t\t\tPID\tPPID\n");
	DIR *procdir = opendir("/proc/");
	struct dirent *dirstr;
	while((dirstr = readdir(procdir))!=NULL)//遍历进程文件夹下面的纯数字文件夹
	{
		if(isprocdir(dirstr->d_name)==1)
		{
			char stafile[500];
			sprintf(stafile,"/proc/%s/status",dirstr->d_name);
			FILE* fp = fopen(stafile,"r");
			//按行读取，截取有用的内容并打印：进程名，pid。ppid
			char msg[20];
			fgets(msg,99,fp);
			char *p = strtok(msg,"\t");
			p = strtok(NULL,"\t");
			p[strlen(p)-1]='\0';
			printf("%-30s",p);
			fgets(msg,99,fp);
			fgets(msg,99,fp);
			fgets(msg,99,fp);
			fgets(msg,99,fp);
			fgets(msg,99,fp);
			p = strtok(msg,"\t");
			p = strtok(NULL,"\t");
			p[strlen(p)-1]='\0';
			printf("%13s",p);
			fgets(msg,99,fp);
			p = strtok(msg,"\t");
			p = strtok(NULL,"\t");
			p[strlen(p)-1]='\0';
			printf("%9s\n",p);
		}

	}



}


int main ()
{

	printf("Welcome to the MHR's shell\n");
	//获取主机名和路径
	gethostname(hostname,sizeof(hostname));
	getcwd(ppath,sizeof(ppath));
	while(1)
	{
		printf("MHR@%s:%s ",hostname,ppath);
	    	char cmd[50] = {0};
	    	fgets(cmd,49,stdin);
	    	cmd[strlen(cmd)-1] = 0;
	    	if(strlen(cmd)==0)
	    		continue;
		char *cmd_set[10] = {0,0,0,0,0,0,0,0,0,0};
		char *p = strtok(cmd," ");//截取命令
		int pos = 0;
		while(p!=NULL && pos < 10)//获取参数
		{
			cmd_set[pos] = p;
			p = strtok(NULL," ");
			pos++;
		}
		if(strcmp(cmd,"run")==0)//run
		{
			runnnn(cmd_set);
		}
		else if(strcmp(cmd,"list")==0)//list
		{
			printinfo();
		}
		else if(strcmp(cmd,"kill")==0)//kill
		{
			kill(atoi(cmd_set[1]),SIGKILL);//杀死参数进程
		}
		else if(strcmp(cmd,"quit")==0)//quit
		{
			printf("Thank you for using!\n");
			return 1;
		}
		else
		{
			printf("UNKNOWN CMD!\n");
		}
	}
	return 0;
}



void runnnn(char *p[10])//参数结构：run    文件名     (setCPU)     (CPU)
{
    int pid=0;
    pid=fork();//fork
    if (pid < 0)
        printf("error in fork!");
    else if (pid == 0) {
        if(p[2]!=NULL)
        {
		if(strcmp("-C",p[2])==0)//设定CPU
		{
			printf("setCPU  %s\n",p[3]);
			//改变CPU亲和度
			//cpu_set_t mask;
			//CPU_ZERO(&mask);
			//CPU_SET(atoi(p[3]),&mask);
			//sched_setaffinity(0,sizeof(cpu_set_t),&mask);
			//函数不识别，功能暂时注释
        	}
        	else
        	{
        		printf("UNKNOWN CMD!\n");
        		return;
        	}
        }
        char *argv[]={p[1]};
        char *envp[]={0,NULL};
        execve(p[1],argv,envp);//运行
    }
    return;
}
