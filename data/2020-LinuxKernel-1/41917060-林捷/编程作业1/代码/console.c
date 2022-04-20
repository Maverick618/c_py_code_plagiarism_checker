#include <stdio.h>
#include <stdlib.h>

#include <string.h>

void run(char *cmd)//

执行不同的命令{	
	//printf("command=%s\n",cmd);
	system(cmd);
}
int main()
{
	char cmd[20];
	while(1)//模拟任务台，不断输入命令
知道输入quit	{	
		printf(">>");
		fgets(cmd,20,stdin)\\输入命令;
		//printf("%s,%ld\n",cmd,strlen(cmd));
		if(strcmp(cmd,"quit\n")==0)
		{	
			printf("quit success!!\n");
			return 0;
		}
		//system("ls");
		run(cmd);
		printf("\n");		
	}

}
