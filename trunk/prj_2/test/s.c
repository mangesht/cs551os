#include<string.h>
#include<stdio.h>

int main() { 

   int res, res1, res2, res3, retVal, test_id = 1 ;
   int * mb_list = NULL;
   int * sender_list = NULL;
   char * msg;
   int i = 0;
   printf ( " ---------Test %d: Creating mail box with All permission--------\n", test_id);
   res = create_mailbox(0x7);
      if(res == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with self permission \n" );
   }else {
          printf("Success: create mail box with self permission returned  %d ",res);
   }
  msg = (char * ) malloc(256);
  printf("Retrieving message ");
  res1 = retrieve(1,msg); 
  if(res1 == -1 ) { 
      printf("Retrieve failed returned %d \n",res1);
  }else{
      printf("Retrieve success returned %d \n",res1);
      for(i=0;i<5;i++) {
        printf("%c",msg[i]);
      }
     printf("\n");
  }
      retVal = destroy_mailbox(res);
      if(retVal == -1){
          printf( "SYSTEM CALL FAIL: Failed to delete mail box\n" );
     }else {
	   printf("Success: delete mail box  returned  %d \n",retVal);
     }   
    return 0;
}
