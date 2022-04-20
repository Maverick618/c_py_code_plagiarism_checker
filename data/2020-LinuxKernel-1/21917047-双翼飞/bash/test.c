#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
extern char **environ;
int main(int argc, char *argv[])
{
    printf("hello world!\n");
    return 0;
}
