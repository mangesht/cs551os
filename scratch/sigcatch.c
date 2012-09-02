#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>

void handler(int snum){
    printf("Signal received \n");
    exit(0);
}

int main(){
int j=0;
printf("Waiting for signal\n");
if(signal(SIGINT,handler) == SIG_IGN)
    signal(SIGINT,SIG_IGN);
printf("Done Waiting for signal\n");

while(j<10000000) { 
    j++;
    printf(" %d\n",j);
    sleep(1);
}
        
return 0;
}
