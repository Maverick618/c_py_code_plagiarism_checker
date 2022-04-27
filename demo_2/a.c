#include<stdio.h>

int add(int x, int y){
    return x + y;
}

int main(){
    int a, b;
    scanf("%d %d", &a, &b);
    return add(a, b);
}