#include<stdio.h>

int main(){
    char *p1;
    char **args[4];
    int i;
    int p_ret;
    int pid;
    int status;
    p1 = (char *) malloc(256);

    for(i=0;i<4;i++){
        args[i] = (char **) malloc(4*sizeof(char *));
    }
    for(i=0;i<4;i++){
        *(args[i] + i) = (char *) malloc(256);
    }
    p1 = "/bin/ls"; 
    args[0][0] = p1;
    args[0][1] = "-a";
    args[0][2] = "-l";
    args[0][3] = NULL;
    args[1][0] = "l";
    args[1][1] = NULL;
    args[1][2] = "n1";
    //execl(p1,p1,"-la",0);
    //execl(p1,p1,args[0][0],args[0][1],0);
    if((pid=fork())!= 0) { 
        int d ; 
        d = 0 ; 
        waitpid(pid,&status,0);
        printf("Process returned %x \n",p_ret);
        printf("Status = %d \n",status);
    }
    else 
        p_ret = execv(p1,args[0],0);
    return 0;
}
