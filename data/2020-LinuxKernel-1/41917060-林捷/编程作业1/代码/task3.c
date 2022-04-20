#include <stdio.h>
#include <unistd.h>

int main(int argc,char* argv[])

{
	
	printf("this is task 3\n");
	
	int a = (int)*argv[1] - 48;
	
	int b = (int)*argv[2] - 48;	
	int mul = a * b;
	printf("execute mul:%d*%d=%d\n",a,b,mul);
	while(1)

	{

	}

	return 1;

}
