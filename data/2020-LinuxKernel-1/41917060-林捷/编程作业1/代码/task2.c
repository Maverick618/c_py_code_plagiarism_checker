#include <stdio.h>
#include <unistd.h>
int main(int argc, char* argv[])
{
	printf("this is task2\n");
	int a = (int)*argv[1] - 48;
	int b = (int)*argv[2] - 48;
	int sub = a - b;
	printf("execute sub:%d-%d=%d\n",a,b,sub);
	while(1)
	{
	}
	return 1;

}
