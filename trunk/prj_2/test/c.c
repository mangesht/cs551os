#include<stdio.h>
#include <string.h>

int main(){
   int res, res1, res2, res3, retVal, test_id = 1 ;
   int * mb_list = NULL;
   int * sender_list = NULL;
   char* msg; 
   int *destin;
   int i = 0;
   printf ( "\n\n------ Test  %d: get_av_mailboxes------\n", test_id);

   mb_list = (int * ) malloc (10 *2*sizeof(int));
   //mb_list[30];
   retVal = get_av_mailboxes(mb_list);
  if(retVal == -1){
	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );

   }else {
          printf("Success: get_av_mailboxes retVal =  %d\n", retVal );
	 for ( i = 0 ; i < 20 ; i ++ ) {
           if(mb_list[i] == -1) break;
		   printf("mb_ %d _id  = %d \t mb_%d_procID = %d\n", i, *(mb_list + (2*i)), i, *(mb_list +(2* i)+1) );
        }
  
   }
   res = mb_list[0];
   printf ( "\n\n------ Test  %d: Register mailbox VALID case------\n", test_id);
      retVal = register_mb(res);
      if(retVal == -1){
          printf( "FAIL: Failed to register mail box\n" );
     }else if(retVal == 0){
	   printf("Success: register  mail box  returned \n");
     }else if(retVal == -2){
	   printf("Success: Sender already Registered \n");
     }
     
    printf("Deposting now \n");
    destin = (int *) malloc(40*sizeof(int));
    destin[0] = mb_list[1];
    destin[1] = -1;
    msg = (char * ) malloc(256);
    strcpy(msg,"Hellow World"); 
    int nTimes;
    for(nTimes=0;nTimes<1;nTimes++){ 
        printf("\nEnter message to send ");
        scanf("%s",msg);
        retVal = deposit(destin,msg);
    }
   return 0;
}
