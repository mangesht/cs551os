#include <stdio.h>
#define STD_INPUT 0
#define STD_OUTPUT 1 
//pipeline(process1,process2)
int main(int argc,char *argv[])
{
    int fd[2];
    char *process1,*process2;
    process1 = (char *) malloc(256);
    process2 = (char *) malloc(256);
    if(argc < 2 ) {
        printf("less number of inputs");
        return 1;
    }else{
        process1 = argv[1];
        process2 = argv[2];
    }
    printf("Running %s | %s \n",argv[1],argv[2]);
    printf("Running %s | %s \n",process1,process2);
    pipe(&fd[0]);
    if(fork()!=0){
        // This is parent 
        close(fd[0]); // Close Read 
        close(STD_OUTPUT);
        dup(fd[1]); // Write pipe port is copied to STD_OUTPUT 
                    // So that the write happens to pipe 
        close(fd[1]);
        execl(process1,process1,0);
    }else{
        // The child process 
        close(fd[1]); // Close write port 
        close(STD_INPUT);
        dup(fd[0]);
        close(fd[0]);
        execl(process2,process2,0);
    }
return 0;
}
