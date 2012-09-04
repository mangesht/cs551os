#include<stdio.h>

int main(){
    char *p1;
    p1 = (char *) malloc(256);
    p1 = "/bin/ls"; 
    execl(p1,p1,"-la",0);
    return 0;
}
