#include <stdio.h>
#include <unistd.h>

int main(int argc,char* argv[])

{
	
	printf("this is task 4\n");
	int a = (int)*argv[1] - 48;
	int b = (int)*argv[2] - 48;
	int div = a / b;
	printf("execute div:%d/%d=%d\n",a,b,div);
	while(1)
	{
	}

	return 1;

}
