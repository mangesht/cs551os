#include <stdio.h>
#define STD_INPUT 0
#define STD_OUTPUT 1 
//pipeline(process1,process2)
int main(int argc,char *argv[])
{
    int fd[2];
    int i;
    int j;
    char *process1,*process2;
    char *prs[4];
    char **args[4];
    process1 = (char *) malloc(256);
    process2 = (char *) malloc(256);
    for(i=0;i<4;i++){
        prs[i] = (char *) malloc(256);
        args[i] = (char **) malloc(4*sizeof(char *));
    }
    for(i=0;i<4;i++){
        *(args[i] + i) = (char *) malloc(256);
    }
    args[0][0] = "-l";
    args[0][1] = NULL;
    args[0][2] = NULL;
    args[1][0] = "l";
    args[1][1] = NULL;
    args[1][2] = "n1";
    for(i=0;i<2;i++) { 
        for(j=0;j<3;j++) {
            printf("options for %d %d is %s\n",i,j,args[i][j]);
        }
    }
    if(argc < 0 ) {
        printf("less number of inputs\n");
        return 1;
    }else{
        process1 = "/bin/ls"; 
        process2 = "/bin/grep";
    }
    printf("Running %s | %s \n",process1,process2);
    pipe(&fd[0]);
    if(fork()!=0){
        // This is parent 
        close(fd[0]); // Close Read 
        close(STD_OUTPUT);
        dup(fd[1]); // Write pipe port is copied to STD_OUTPUT 
                    // So that the write happens to pipe 
        close(fd[1]);
        execl(process1,process1,args[0],0);
    }else{
        // The child process 
        close(fd[1]); // Close write port 
        close(STD_INPUT);
        dup(fd[0]);
        close(fd[0]);
        execl(process2,process2,args[1],0);
    }
return 0;
}
