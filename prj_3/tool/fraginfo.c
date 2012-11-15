#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void display_help(){
   printf("\n Usage : fileinfo filename [-p] [-b] [-h] \n");
   printf("     -p  : gets the information of processes using the file filename \n");   
   printf("     -b  : gets the information of disk blocks used by the file filename \n");   
   printf("     -h  : gets the help how to use this tool \n"); 
}
int main(int argc,char *argv[]) { 
   int retVal;
   int mode_i = 0  ;
   int agcCount =1 ;
   int got_f_name;
   int got_opt;
   char *p;
   char *fname;
   p = (char *) malloc (256);
   fname =  (char *) malloc (256);
   //strcpy(p , argv[agcCount]);
   //printf("Atgc = %d \n",argc);
   got_f_name = 0 ;
   got_opt = 0 ;
   // This is default filename 
   strcpy(fname, "/home/mthakare/test/sample.txt");
   while(agcCount < argc){ 
     strcpy(p , argv[agcCount]);
     //printf("p = %s C= %c",p,p[0] );
     //printf("agcCount = %d ",agcCount);
     //printf("\n");
     if(p[0] == '-'){ 
        //printf("option selected \n");
        got_opt = 1 ;
        if(strchr(p,'p') != NULL) {
            //printf("Process selected \n");
            mode_i = mode_i | 0x1 ;
        }
        if(strchr(p,'b') != NULL) {
            //printf("Block selected \n");
            mode_i = mode_i | 0x2 ;
        }
        if(strchr(p,'h') != NULL) {
            //printf("Help selected \n");
            display_help();
            return 0;
        }
        
     }else{
         // This must be filename
         if(got_f_name == 1 ) {
             display_help();
             return 0;
         }
         strcpy(fname,p);
         got_f_name = 1 ;
     }
     agcCount++;   
   } 

   if(got_opt ==0 ) { 
        mode_i = 3;
   }
   //printf("FileName in test = %s \n",fname);
  mode_i = 0x4 ;
   retVal = fileinfo(fname,mode_i);
   if(retVal == -1) { 
      perror("fileinfo:");
   }
return 0;
} 
