#include <stdio.h>
#include <stdlib.h>

#include <string.h>

void run(char *cmd)//

ִ�в�ͬ������{	
	//printf("command=%s\n",cmd);
	system(cmd);
}
int main()
{
	char cmd[20];
	while(1)//ģ������̨��������������
֪������quit	{	
		printf(">>");
		fgets(cmd,20,stdin)\\��������;
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
