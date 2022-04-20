#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
#define MAX_HISTORY_NUM 10 // 最多纪录历史命令的个数
#define LINE_MAX 100
#define PS_COMMAND "ps -au"
#define TASKSET_COMMAND "taskset"
#define TASKSET_ARGS "-c"

/*
计算最小值
*/
int min(int a, int b) {
	return a < b ? a : b;
}


/*将str1字符串中第一次出现的str2字符串替换成str3*/
void replace(char *str1,char *str2,char *str3)
{
    char str4[strlen(str1)+1];
    char *p;
    strcpy(str4,str1);
    if((p=strstr(str1,str2))!=NULL)/*p指向str2在str1中第一次出现的位置*/
    {
        while(str1!=p&&str1!=NULL)/*将str1指针移动到p的位置*/
        {
            str1++;
        }
        str1[0]='\0';/*将str1指针指向的值变成/0,以此来截断str1,舍弃str2及以后的内容，只保留str2以前的内容*/
        strcat(str1,str3);/*在str1后拼接上str3,组成新str1*/

		/*strstr(str4,str2)是指向str2及以后的内容(包括str2),strstr(str4,str2)+strlen(str2)就是将指针向前移动strlen(str2)位，跳过str2*/
        strcat(str1,strstr(str4,str2)+strlen(str2));
    }
}

/*
split user input strings then store in args[][]
strtok() 用于分解字符串，这里使用空格进行分解
*/
int split_command(char *commandline, char ** args, int *cmd_flag) {

	int cnt = 0;
	char temp[LINE_MAX];
	char temp1[LINE_MAX];
	int i=0;

	*cmd_flag = 0;
	strcpy(temp, commandline);
	/* 获取到第一个参数，判断是否是自定义的命令，将命令进行转换
		list == ps -au
		run == taskset
	*/
	args[cnt] = strtok(temp, " ");
	if (strcmp(args[0], "quit") == 0) {
		*cmd_flag = 1;
		return 1;
	} else if (strcmp(args[0], "list") == 0) {
		*cmd_flag = 2;
		strcpy(commandline, PS_COMMAND);
	} else if (strcmp(args[0], "kill") == 0) {
		*cmd_flag = 3;
	} else if (strcmp(args[0], "run") == 0) {
		printf("you are use run command\n");
		// 将run 命令替换为 taskset
		replace(commandline, "run", TASKSET_COMMAND);
		*cmd_flag = 4;
	}

	/* 将命令进行分割 */
	args[cnt] = strtok(commandline, " ");
	while (args[cnt] != NULL) {
		args[++cnt] = strtok(NULL, " ");
	}
	args[cnt] = NULL;

#if 0
	printf("the end of command is \n");
	for(i=0; i<cnt; i++)
		printf("%d, %s\n", i, args[i]);
#endif

	return cnt;
}

/*
1. 运行后固定在终端输出 thcbash> 做为prompt;
2. 调用 get() 函数获取用户输入;
3. 调用 split_command() 函数通过空格进行命令的解析，将参数个数赋值给argc，参数赋值给argv[][];
4. 之后创建子进程然后调用execvp()执行对应的命令
*/
int main() {
	char* args[LINE_MAX / 2 + 1];
	int history_cnt = 0;
	int cmd_flag = 0;

	printf("This is tanghaocheng bash\n");
	fflush(stdout);
	while (1) // 如果用户输入 quit,就会跳出死循环并结束
	{
		printf("thcbash> "); // 显示 thcbash 的prompt
		fflush(stdout);

		char commandline[LINE_MAX], tmp[LINE_MAX]; // commandline[] 用来存储用户输入字符串，tmp[]用来进行临时存储

		gets(commandline); // 等待用户的输入
		strcpy(tmp, commandline);
		int cnt = split_command(tmp, args, &cmd_flag); // 进行命令分割，确定何种命令，cnt为命令的个数

		if (cnt == 1) {
			if (cmd_flag == 1) {  // 退出运行，跳出循环
				printf("exist bash !!!!!! \n");
				break;
			}
		}

		pid_t pid;
		/* 创建一个子进程用来执行用户输入的命令 */
		pid = fork();
		if (pid < 0) {
			fprintf(stderr, "Fork Failed");
			return 1;
		} else if (pid == 0) { // 子进程
			execvp(args[0], args); // 调用execvp 去执行命令
			return 0;
		} else { // 父进程
			// 父进程等待子进程执行完毕
			wait(NULL);
			//printf("Child Complete: pid = %d\n", pid);
			//printf("return to commandline ...\n");
		}

	}
	return 0;
}

