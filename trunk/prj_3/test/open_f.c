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
   char fname[250];//= "/home/mthakare/test/sample.txt";
   if(argc>1) {
      strcpy(&fname,argv[1]);
   }else{
      strcpy(&fname, "/home/mthakare/test/sample.txt");
   }
   printf("FileName in test = %s \n",fname);

   fd = open(fname,O_RDONLY);
   if(fd == -1 ) { 
     printf("Open failed \n");
     perror("open:");
   }
   printf("waiting to close , enter 1 to close \n");
   scanf("%d",&w);
   close(fd); 
   return 0;
}
