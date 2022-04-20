/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   findNumInRandomArray.c
 * Author: zch
 *
 * Created on 2020年6月16日, 下午2:15
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*
 * 
 * argv[0]:self
 * argv[1]:N
 * argv[2]:num
 */
int N;
int num;
int* arr;
int i;
int main(int argc, char** argv) {
    
    if(argc!=3){
        printf("insufficient number of parameters!");
        return -1;
    }
    N=atoi(argv[1]);
    if(N==0){
        printf("illegal N!");
        return -1;
    }
    srand((unsigned)time(NULL));
    arr=(int*)calloc(N,sizeof(int));
    
    for(i=0;i<N;i++){
        arr[i]=rand()%N;
    }
    num=atoi(argv[2]);
    for(i=0;i<N;i++){
        if(num==arr[i])
        {
            printf("found %d at position %d in arr\n",num,i);
            return 1;
        }
    }
    printf("did not found %d in arr\n",num);
    return (EXIT_SUCCESS);
}

