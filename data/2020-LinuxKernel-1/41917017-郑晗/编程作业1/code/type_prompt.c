
#include "myshell.h"

void type_prompt(char *prompt)
{
    int length;
    sprintf(prompt,"[Myshell]:");
    
    length = strlen(prompt);
    
    sprintf(prompt+length,"$");
    return;
}

