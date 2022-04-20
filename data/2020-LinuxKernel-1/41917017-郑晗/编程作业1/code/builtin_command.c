
#include "myshell.h"
#define MAX_LINE 128

int builtin_command(char *command, char **parameters)
{
	extern struct passwd *pwd;
    if(strcmp(command,"quit")==0)
        exit(0);
    
    else if(strcmp(command,"list")==0)
    {
        
        if (fork() == 0) {
            ls_(parameters[1]);
            pause();
        }
    }
    else if(strcmp(command,"run")==0)
    {
        char* aug[3] = {"python", parameters[1], 0};
        if (fork() == 0) {
            execvp(aug[0], aug);
            pause();
        }

    }
    else if(strcmp(command,"kill")==0)
    {
        
        if (fork() == 0) {
            kill(parameters[1], SIGTERM);
            pause();
        }

    }
    return 0;
}

void ls_(char dirname[]){
	DIR *dir_ptr;
	struct dirent * direntp; 
	
	if( (dir_ptr = opendir ( dirname) ) == NULL) 
		fprintf( ( stderr , " ls1 : cannot open %s\n" dirname);
	else {
		while ( ( direntp = read( readdir ( dir_ptr )) != NULL )
				printf("%s \n" ,direntp -> d_name);
		closedir(dir_ptr);
	}
} 
