/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   countDown.c
 * Author: zch
 *
 * Created on 2020年6月18日, 下午12:43
 */

#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
/*
 * 
 */
int main(int argc, char** argv) {
    int count=atoi(argv[1]);
    while(count>=0){
        freopen("timeremaining.txt","w",stdout);
        printf("剩余时间:%d\n",count);
        count--;
        //printf("\033[1A");
        fclose(stdout);
        sleep(1);
    }
    return (EXIT_SUCCESS);
}

