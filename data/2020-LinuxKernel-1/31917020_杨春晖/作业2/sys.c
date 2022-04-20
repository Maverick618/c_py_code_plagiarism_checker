#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
int main()
{
    int flag =1;
    int act;
    char cmd1[40]="taskset -c ";
char cpu[40]="taskset -c 1 ./";
char cmd2[20];
int c;
    int pid;
    while(flag){
    printf("please choose ur act 1/2/3/4 : \r\n");
    printf("no.1 list all \r\n");
    printf("no.2 run on cpu\r\n");
    printf("no.3 kill process\r\n");
    printf("no.4 exit system\r\n");
    scanf("%d",&act);
    //printf("please chose act : %d \r\n",act);
    if(act == 1){
       printf("runing process: \r\n");
       system("ps -u user -f");
    }else if(act==2){
       printf("choose cpu: ");
       scanf("%d",&c);
       //char cc = '2';
       char cc= (char)(c+'0');
       cpu[11]=cc;
       printf("choose programs: ");
       scanf("%s",cmd2);
       strcat(cpu,cmd2);
       puts(cpu);
       //strcat(cmd1,cpu);
       //puts(cmd1);
       system(cpu);
    }else if(act ==3){
       printf("input pid: \r\n");
       scanf("%d",&pid);
       int status = kill(pid,SIGKILL);
       if(status==-1){
          printf("failed \r\n");
       }else{
          printf("killed: \r\n");
       } 
    }else{
       flag=0;
       printf("thx bye ");
    }
  }

    return 0;
}
