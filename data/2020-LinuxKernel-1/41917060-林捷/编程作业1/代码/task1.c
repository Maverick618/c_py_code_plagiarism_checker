#include <stdio.h>
#include <unistd.h>

int main(int argc,char* argv[])

{
	printf("this is task 1\n");
	int a = (int)*argv[1] - 48;
	int b = (int)*argv[2] - 48;	
	int sum = a + b;
	printf("execute sum:%d+%d=%d\n",a,b,sum);

	while(1)

	{


	}

	return 1;

}
