#define _GNU_SOURCE
#include<stdio.h>                                                                                                                       
#include<unistd.h>
#include<stdlib.h>
#include<wait.h>
#include<string.h>
#include <sched.h>
void ParseArg(char* input, char* output[])  
{
    char* p = strtok(input," ");
    int output_size = 0;
    while(p != NULL)
    {
        output[output_size++] = p;
        p = strtok(NULL," ");
    }
    output[output_size] = NULL;
}
int iscore(char *argv[])
{
    int n=0;
    while(argv[n]!=NULL)
    {
	n++;
    }
    if(strstr(argv[n-1],"core")!=NULL)
    {
	return n-1;
    }
    else
    {
	return 0;
    }
}
void Exec(char *argv[])
{  
    char path[100] = {0};
    getcwd(path,sizeof(path)); 
    char *envp[]={path,NULL};

    pid_t pid = fork();
    if(pid==-1)
    {
	perror("fork()");
    }
    else if(pid==0)
    {
	int n=iscore(argv);
 	int cpu_core_enable=sysconf(_SC_NPROCESSORS_CONF);
	if(n<cpu_core_enable)
	{
	    int cpu_num=argv[n][4]-'0';
	    cpu_set_t mask;
	    CPU_ZERO(&mask);
	    CPU_SET(cpu_num, &mask);
	    sched_setaffinity(0, sizeof(cpu_set_t), &mask);
            printf("sched_getcpu():%d\n", sched_getcpu());
	}
	else
	{
	    printf("we only have %d cores\n",cpu_core_enable);
	    printf("sched_getcpu():%d\n", sched_getcpu());
	}
    	execve(argv[0],argv,envp);
        perror("execvp()");
        exit(0);
    }
    else
    {
	wait(NULL);
    }
}
void list()
{
    int i=0;
    while(i++<10000){
	get_proc(i);
    }
}
void get_proc(int pid)
{
    char filename[128];
    char buff[1024];
    int str_len;
    FILE *fp;
    sprintf(filename, "/proc/%d/status", pid);
    fp = fopen(filename, "r");
    if( fp != NULL ){
        str_len = fread(buff, 1, 1024, fp);
        if( str_len > 0) {
	    printf("%s\n",buff);
            fclose(fp);
        }
    }
}
void kill_proc(int pid)
{
	int status;
	status=kill(pid,SIGKILL);
	if (status == -1)
		printf("kill faild\n");
	wait(&status);
	if(WIFSIGNALED(status))
		printf("chile process receive signal %d\n",WTERMSIG(status));
}
int main()
{
    int flag=1;
    while(flag)
    {
	char input[100];
	printf("[MyShell]");
	fgets(input,sizeof(input),stdin);
	if(strstr(input,"quit")!=NULL)break;
	char* argv[100];  
	ParseArg(input,argv);
	if(strstr(input,"./")!=NULL)
	{
    	    Exec(argv);
	}
	else if(strstr(input,"list")!=NULL)
	{
	    list();
	}
	else if(strstr(input,"kill")!=NULL)
	{
	    if(argv[1]!=NULL)
	    {
	        int pid=atoi(argv[1]);
	        kill_proc(pid);
	    }
	    else
	    {
		printf("please input you want to kill the process's pid!\n");
	    }
	}
	else
	{
	    printf("Command \'%s\' not found!",input);
	}
    }
    return 0;
}


