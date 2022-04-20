/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include<signal.h>
void sig_handler(int signum){
    fclose(stdout);
    exit(0);
}
int main(){
    signal(SIGTERM,sig_handler);
    while(1){
        time_t timer;
        struct tm *tblock;
        timer=time(NULL);
        tblock=localtime(&timer);
        //printf("%s","\033[1H\033[2J");   //在Terminal中打印这个就是清屏
        //printf("%s","\r\033[K");
        //system("clear");
        freopen("time.txt","w",stdout);
        printf("%s",asctime(tblock));
//        printf("\033[1A");
        fclose(stdout);
        sleep(1);
    }
    return 0;
}
