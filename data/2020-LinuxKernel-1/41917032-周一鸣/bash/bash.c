#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<errno.h>
#include<dirent.h>
#include<signal.h>
#include<sched.h>
#include<sys/types.h>
#include<pwd.h>
#include<sys/utsname.h>
#include<sys/wait.h>
#include<sys/stat.h>

#define CMDLEN 128
#define NUM 20
#define MAX_LEN 20

//[用户名@主机名  当前工作目录]标识符
void PrintInfo()
{
	struct passwd *pw = getpwuid(getuid());//获取用户名
	assert(pw != NULL);
	
	struct utsname host;//获取主机名
	uname(&host);

	char path[CMDLEN] = { 0 };
	getcwd(path, CMDLEN - 1);//getcwd函数来确定自己的当前工作目录,其中path为绝对路径
	//获取绝对路径的最后一个路经名称
	char* dirname = NULL;
	if (strcmp(path, pw->pw_dir) == 0)//路经如果为家目录
	{
		dirname = "~";
	}
	else
	{
		dirname = path + strlen(path);
		while (*dirname != '/')
		{
			dirname--;
		}
		if (strlen(path) != 1)//如果是根目录 /
		{
			dirname++;
		}
	}
	char flag = '$';
	if (getuid() == 0)//0是root用户的ID
	{
		flag = '#';
	}

	printf("[%s@%s %s]%c", pw->pw_name, host.nodename, dirname, flag);

}

//将命令与各参数的分割，分别存储到字符串指针数组中
void CutCommand(char *cmd, char *cmdArr[])
{
	char *p = strtok(cmd, " ");
	int index = 0;
	while (p != NULL && index < NUM)
	{
		cmdArr[index++] = p;
		p = strtok(NULL, " ");
	}
}

void RunCommand(char *path, char *cpu){
    int pid;
    int cpuNum = sysconf(_SC_NPROCESSORS_CONF);
    pid = fork();   
    if (pid == 0){
        if(atoi(cpu) < 0 || atoi(cpu) > cpuNum){
            fprintf(stderr,"cpu num error:%s",strerror(errno));
        }   
        else{
            cpu_set_t set;
            CPU_ZERO(&set);
            CPU_SET(atoi(cpu), &set);
            if(sched_setaffinity(pid, sizeof(set), &set) == -1){
                fprintf(stderr,"could not set cpu:%s",strerror(errno));
            }
        }
        if(execlp(path, NULL)<0){
            fprintf(stderr,"run failed:%s",strerror(errno));
            exit(0);
        }
    }
    else{
		//前后台处理
		wait(NULL);//前台，后台要处理僵死进程
    }
}

void DealExec(char *cmdArr[])
{
	pid_t pid = fork();
	assert(pid != -1);

	if (pid == 0)
	{
		//用户给定一个命令   ls  pwd  su  ps
		//用户想执行一个程序  ./main  ./mybash
		char file[CMDLEN] = { 0 };
		if (strstr(cmdArr[0], "/") != NULL)
		{
			strcpy(file, cmdArr[0]);
		}
		else
		{
			strcpy(file, "/bin/");
			strcat(file, cmdArr[0]);
		}
		execv(file, cmdArr);//如果命令不在，则execv会失败
		perror("execv");
		exit(0);//如果替换失败，子进程要能正常结束
	}
	else
	{
		//前后台处理
		wait(NULL);//前台，后台要处理僵死进程
    }
}

/*
*  ps命令实现
*/
struct ps_info *trav_dir(char dir[]);
int read_info(char d_name[],struct ps_info *p1);
void print_ps(struct ps_info *head);
int is_num(char *);
void uid_to_name();
 
typedef struct ps_info
{
    char pname[MAX_LEN];
    char user[MAX_LEN];
    int  pid;
    int  ppid;
    char state;
    struct ps_info *next;
}mps;
 
void ListCommand()
{
    mps *head,*link;
     
    head=trav_dir("/proc/");
    if(head==NULL)
        printf("traverse dir error\n");
    print_ps(head);
 
    while(head!=NULL)        //释放链表
    {
        link=head;
        head=head->next;
        free(link);
    }
}
 
mps *trav_dir(char dir[])                         
{
    DIR *dir_ptr;
    mps *head,*p1,*p2;
    struct dirent *direntp;
    struct stat infobuf;
 
    if((dir_ptr=opendir(dir))==NULL)
        fprintf(stderr,"dir error %s\n",dir);
    else
    {
        head=p1=p2=(struct ps_info *)malloc(sizeof(struct ps_info));    //建立链表
        while((direntp=readdir(dir_ptr)) != NULL)               //遍历/proc目录所有进程目录
        {
            if((is_num(direntp->d_name))==0)                   //判断目录名字是否为纯数字
            {
                if(p1==NULL)
                {
                    printf("malloc error!\n");
                    exit(0);
                }
 
                if(read_info(direntp->d_name,p1)!=0)         //获取进程信息
                {
                    printf("read_info error\n");
                    exit(0);
                }
                p2->next=p1;                        //插入新节点
                p2=p1;
                p1=(struct ps_info *)malloc(sizeof(struct ps_info));
            }
        }
    }
    p2->next=NULL;
    return head;
}
 
int read_info(char d_name[],struct ps_info *p1)
{
    FILE *fd;
    char dir[20];
    struct stat infobuf;
 
    sprintf(dir,"%s/%s","/proc/",d_name);
    chdir("/proc");                        //切换至/proc目录，不然stat返回-1
    if(stat(d_name,&infobuf)==-1)                     //get process USER
        fprintf(stderr,"stat error %s\n",d_name);
    else
        //p1->user=uid_to_name(infobuf.st_uid);
        uid_to_name(infobuf.st_uid, p1);
 
    chdir(dir);                         //切换到/proc/pid目录
    if((fd=fopen("stat","r"))<0)
    {
        printf("open the file is error!\n");
        exit(0);       
    }
    while(4==fscanf(fd,"%d %s %c %d\n",&(p1->pid),p1->pname,&(p1->state),&(p1->ppid)))     //读文件的前四个字段内容，并存放在相关的链表成员中
    {
        break;                                    
    }
    fclose(fd);
    return 0;
}
 
void uid_to_name(uid_t uid, struct ps_info *p1)         //由进程uid得到进程的所有者user
{
    struct passwd *getpwuid(), *pw_ptr;
    static char numstr[10];
 
    if((pw_ptr=getpwuid(uid)) == NULL)
    {
        sprintf(numstr,"%d",uid);
        strcpy(p1->user, numstr);
    }
    else
        strcpy(p1->user, pw_ptr->pw_name);
}
 
int is_num(char p_name[])
{
    int i,len;
    len=strlen(p_name);
    if(len==0) return -1;
    for(i=0;i<len;i++)
        if(p_name[i]<'0' || p_name[i]>'9')
            return -1;
    return 0;
}
 
void print_ps(struct ps_info *head)
{
    mps *list;
    printf("USER\t\tPID\tPPID\tSTATE\tPNAME\n");
    for(list=head;list!=NULL;list=list->next)
    {
        printf("%s\t\t%d\t%d\t%c\t%s\n",list->user,list->pid,list->ppid,list->state,list->pname);
    }
}

/*
* Ps结束
*/

void KillCommand(char *cmdArr){
    kill(atoi(cmdArr), SIGKILL);
}

int main()
{
	while (1)
	{
		//1、打印提示符信息
		PrintInfo();
		//2、获取用户输入的命令
		char cmd[CMDLEN] = { 0 };
		fgets(cmd, 127, stdin);//最后的回车符也在cmd数组中
		cmd[strlen(cmd) - 1] = 0;//将最后一个字符置为0
        
		//空命令
		if (strlen(cmd) == 0)
		{
			continue;
		}
    
		//命令与各参数的分割  例如：ls -a -l  将其3个分别存入3个字符串
		char *cmdArr[NUM] = { 0 };
		CutCommand(cmd, cmdArr);

		//3、划分命令： 空、内置、外置
		//划分命令只与命令有关，与参数无关
		if (strlen(cmdArr[0]) == 3 && strncmp(cmdArr[0], "run", 3) == 0)//run命令
        {
            if (cmdArr[2] == 0)			
                RunCommand(cmdArr[1], NULL);
            else
                RunCommand(cmdArr[1], cmdArr[2]);
		}
        else if (strlen(cmdArr[0]) == 4 & strncmp(cmdArr[0], "list", 4) == 0)//list命令
        {
            ListCommand();
        }
        else if (strlen(cmdArr[0]) == 4 && strncmp(cmdArr[0], "kill", 4) == 0)//kill命令
        {
            KillCommand(cmdArr[1]);
        }
        else if (strlen(cmdArr[0]) == 4 && strncmp(cmdArr[0], "quit", 4) == 0)//exit命令
        {
            exit(0);//结束进程
        }
        else//外置命令
        {
            DealExec(cmdArr);
        }
    }
}