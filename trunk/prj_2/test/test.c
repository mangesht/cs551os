#include<stdio.h>
#include <string.h>

int test_suite1(){
   int res, res1, res2, res3, retVal, test_id = 1 ;
   int * mb_list = NULL;
   int i = 0;
   printf ( " ---------Test %d: Creating mail box with Self permission--------\n", test_id);
   res = create_mailbox(0x4);
   
   if(res == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with self permission \n" );
   }else {
          printf("Success: create mail box with self permission returned  %d ",res);
   }
   
   test_id++;
   printf ( "\n\n ------ Test  %d: Creating mail box with Group permission------\n", test_id);
   res1 = create_mailbox(0x2);
    if(res1 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with group permission \n" );
   }else {
          printf("Success: create mail box with group permission returned  %d ",res1);
   }
   
   test_id++;
   printf ( "\n\n-----Test  %d: Creating mail box with everyone permission------\n", test_id);
   res2 = create_mailbox(0x1);
  if(res2 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with everyone permission \n" );
   }else {
          printf("Success: create mail box with everyone permission returned  %d ",res2);
   }
   
   test_id++;
   printf ( "\n\n------Test  %d: Creating mail box with different permission   ------\n", test_id);
   res3 = create_mailbox(0x9);
  if(res3 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with everyone permission \n" );
   }else {
          printf("Success: create mail box with everyone permission returned  %d ",res3);
   }
   
   
   test_id++;
   printf ( "\n\n------ Test  %d: get_av_mailboxes------\n", test_id);
   mb_list = (int * ) malloc (10 *2*sizeof(int));
   retVal = get_av_mailboxes(mb_list);
  if(retVal == -1){
	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );
   }else {
          printf("Success: get_av_mailboxes retVal =  %d\n", retVal );
	 for ( i = 0 ; i < 20 ; i = i+2 ) {
		   printf("mb_ %d _id  = %d \t mb_%d_procID = %d\n", i, *mb_list, i, *(mb_list +1) );
		   i = i+2; 
           }
  
   }
    
   
   test_id++;
   printf ( "\n\n------ Test  %d: Register mailbox VALID case------\n", test_id);
      retVal = register_mb(res);
      if(retVal == -1){
          printf( "FAIL: Failed to register mail box\n" );
     }else if(retVal == 0){
	   printf("Success: register  mail box  returned \n");
     }else if(retVal == -2){
	   printf("Success: Sender already Registered \n");
     }
     
   test_id++;
   printf ( "\n\n------ Test  %d: Register mailbox Sending already exist------\n", test_id);
      retVal = register_mb(res);
      printf ( "%d", retVal);
      if(retVal == -1){
          printf( "FAIL: Failed to register mail box\n" );
     }else if(retVal == 0){
	   printf("FAIL: register  mail box  returned  \n");
     }else if(retVal == -2){
	   printf("Success: Sender already Registered \n ");
     }
   
   
test_id++;
   printf ( "\n\n------ Test  %d: Register mailbox INVALID case------\n", test_id);
      res = register_mb(10);
     if(res == -1){
          printf( "Success: Failed to register mail box\n" );
     }else {
	   printf("FAIL: Register mailbox returned success");
     }

   
     test_id++;
      printf ( "\n\n------Test  %d: Authorized Delete  mail box of Self permission ------\n", test_id);
      res = destroy_mailbox(res);
      if(res == -1){
          printf( "SYSTEM CALL FAIL: Failed to delete mail box\n" );
     }else {
	   printf("Success: delete mail box  returned  %d ",res);
     }   
     
     test_id++;
      printf ( "\n\n------Test  %d: Authorized Delete  mail box of group permission------\n", test_id);
      res = destroy_mailbox(res1);
      if(res == -1){
          printf( "SYSTEM CALL FAIL: Failed to delete mail box\n" );
     }else {
	   printf("Success: delete mail box  returned  %d ",res);
     }   
   
   test_id++;
    printf ( "\n\n------ Test  %d: Authorized Delete  mail box of Everyone permission------\n", test_id);
      res = destroy_mailbox(res2);
      if(res == -1){
          printf( "SYSTEM CALL FAIL: Failed to delete mail box\n" );
     }else {
	   printf("Success: delete mail box  returned  %d ",res);
     }   
   
   
   
   test_id++;
    printf ( "\n\n------ Test  %d: Authorized Delete  mail box of other permission------\n", test_id);
      res = destroy_mailbox(res3);
      if(res == -1){
          printf( "SYSTEM CALL FAIL: Failed to delete mail box\n" );
     }else {
	   printf("Success: delete mail box  returned  %d ",res);
     }   
     
   
   //Test 7: Tested mailbox full also by executing the same exe 3 times which says  Error: mail box space full
   // Test 8: UnAuthorized delete tested 
}


int main(){
int res;

test_suite1();
//test_delete_mb();

return 0;
}
