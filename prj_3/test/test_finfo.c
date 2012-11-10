#include <stdio.h>

int main() {
   int retVal;
   char fname[] = "/home/mthakare/test/sample.txt";
   printf("FileName in test = %s \n",fname);
   retVal = fileinfo(&fname);
   if(retVal == -1) { 
      printf("Error in getting file info \n");
   }
   return 0;
}
