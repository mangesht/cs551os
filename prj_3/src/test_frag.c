#include <stdio.h>

int main(int argc,char *argv[]) { 
   int retVal;
   int mode ;
   char fname[250];

   if(argc>1) {
      strcpy(&fname,argv[1]);
   } else {
      strcpy(&fname, "/home/raj/cs551os/prj_3/src/sampledata.txt");
   }
  
  if(argc = 2){
   mode =argv[2];
  }

   printf("FileName in test = %s \n",fname);
   retVal = fraginfo(&fname,mode);
   if(retVal == -1) { 
      printf("Error in fraginfo \n");
      perror("error in fraginfo:");
   }
   return 0;
}


// mode = 1  :  external frag of file system
// mode = 2  :  internal frag of file system
// mode = 3  :  external frag for a file
// mode = 4  :  internal frag for a file