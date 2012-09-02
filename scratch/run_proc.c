#include<stdio.h>

int main(){
    char *p1;
    p1 = (char *) malloc(256);
    p1 = "ls"; 
    execl(p1,p1,0);
    return 0;
}
