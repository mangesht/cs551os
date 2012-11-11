#include <stdio.h>

int main(int argc,char *argv[]) { 
   int retVal;
   int mode_i ;
   char fname[250];//= "/home/mthakare/test/sample.txt";
   if(argc>1) {
      strcpy(&fname,argv[1]);
   } else {
      strcpy(&fname, "/home/mthakare/test/sample.txt");
   }
   mode_i = 3 ;
   printf("FileName in test = %s \n",fname);
   retVal = fileinfo(&fname,mode_i);
   if(retVal == -1) { 
//      printf("Error in getting file info \n");
      perror("fileinfo:");
   }
   return 0;
}
