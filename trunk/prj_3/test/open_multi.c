#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>

int main(int argc,char *argv[]) { 
   int fd;
   int w;
   int nTimes = 1;
   int i;
   int sleepTime = 10 ;
   int pid;
   char fname[250];//= "/home/mthakare/test/sample.txt";
   if(argc>1) {
      strcpy(&fname,argv[1]);
   }else{
      strcpy(&fname, "/home/mthakare/test/sample.txt");
   }
   printf("How many times you want to open the file : ");
   scanf("%d",&nTimes);
   printf("FileName in test = %s number of process = %d \n",fname,nTimes);
            fd = open(fname,O_RDONLY);
            if(fd == -1 ) { 
                 printf("Open failed \n");
                 perror("open:");
                 exit(EXIT_FAILURE);
            }
 
   for(i=0;i<nTimes;i++) { 
        pid = fork();
        if(pid == -1) { 
            perror("fork_error");
            exit(EXIT_FAILURE);
        }else if(pid != 0) {
            // Parent process 
        } else {
            // Child opens file 
/*            fd = open(fname,O_RDONLY);
            if(fd == -1 ) { 
                 printf("Open failed \n");
                 perror("open:");
                 exit(EXIT_FAILURE);
            }
*/
            printf("waiting to close \n");
            //scanf("%d",&w);
            sleep(sleepTime);
            close(fd); 
            exit(0);
        }
   }
   return 0;
}
