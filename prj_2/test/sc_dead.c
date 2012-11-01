#include<stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
   int res, res1, res2, res3, retVal, test_id = 1 ;
   int * mb_list = NULL;
   int * sender_list = NULL;
   char* msg; 
   char* msg_rx; 
   int c;
   int *destin;
   int *d_copy;
   int i = 0;
   int mb_id;
   int idx;
   int rx_mb_id;
   
  destin = (int *) malloc(40*sizeof(int));
  d_copy = (int *) malloc(40*sizeof(int));
   printf ( " ---------Test %d: Creating mail box with All permission--------\n", test_id);
   mb_id = create_mailbox(0x7);
      if(mb_id == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with self permission \n" );
   }else {
          printf("Success: create mail box with self permission returned  %d ",mb_id);
   }


   printf ( "\n\n------ Test  %d: get_av_mailboxes------\n", test_id);

   mb_list = (int * ) malloc (10 *2*sizeof(int));
   //mb_list[30];
   rx_mb_id = -1;
   while(rx_mb_id < 0 ) {
       retVal = get_av_mailboxes(mb_list);
       if(retVal == -1){
    	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );
    
       }else {
         printf("Success: get_av_mailboxes retVal =  %d\n", retVal );
    	 for ( i = 0 ; i < 20 ; i ++ ) {
               if(mb_list[i] == -1) break;
    		   printf("own mb id = %d mb_ %d _id  = %d \t mb_%d_procID = %d\n",mb_id, i, *(mb_list + (2*i)), i, *(mb_list +(2* i)+1) );
               if((mb_list[2*i] != mb_id)&& (rx_mb_id < 0)) { 
                    rx_mb_id = mb_list[2*i];
                    destin[0] = mb_list[2*i+1];
                    d_copy[0] = destin[0];
                    printf("Selecting mailbox id = %d \n",rx_mb_id );
                    //break;
                }
            }
      
       }
       sleep(1);
   }
   
//   res = mb_list[0];
   printf ( "\n\n------ Test  %d: Register mailbox VALID case------\n", test_id);
      printf("Registering to mailbox id = %d \n",rx_mb_id);
      retVal = register_mb(rx_mb_id);
      if(retVal == -1){
          printf( "FAIL: Failed to register mail box\n" );
     }else if(retVal == 0){
	   printf("Success: register  mail box  returned \n");
     }else if(retVal == -2){
	   printf("Success: Sender already Registered \n");
     }
     
    printf("Deposting now \n");
    //destin[0] = mb_list[1];
    destin[1] = -1;
    d_copy[1] = -1;
    msg = (char * ) malloc(256);
    msg_rx = (char * ) malloc(256);
    strcpy(msg,"Hellow World"); 
    int nTimes;
    
    for(nTimes=0;nTimes<15;nTimes++){ 
        printf("\nEnter any char to send message");
        scanf("%d",&c);
        //sleep(1);
        for(i=0;1 ; i++) { 
             destin[i] = d_copy[i];
             if(d_copy[i] == -1 ) break;
        }
        retVal = deposit(destin,msg);
        printf("Deposit returned = %d destin = %d \n",retVal,destin[0]);
        if(retVal == -2) { 
             printf("deadlock detected \n");
             // Start receving messages 
             printf("Start getting messages ? Enter 1 \n");
             scanf("%d",&c);
             for(i=0;i<10;i++) {
                 res1 = retrieve(1,msg_rx); 
                 if(res1 == -1 ) { 
                      printf("Retrieve failed returned %d \n",res1);
                  }else{
                      printf("Retrieve success returned %d \n",res1);
                      printf("%s",msg_rx);
                      printf("\n");
                 }
             }
             break;

          } else if(retVal == -1) {
             printf("Deposit failed rx not available\n");
          } 
      }
     for(nTimes=0;nTimes<5;nTimes++){ 
        printf("\nEnter any char to send message");
        scanf("%d",&c);
        //sleep(1);
        for(i=0;1 ; i++) { 
             destin[i] = d_copy[i];
             if(d_copy[i] == -1 ) break;
        }

        retVal = deposit(destin,msg);
        printf("Deposit returned = %d destin = %d \n",retVal,destin[0]);
        if(retVal < 0) { 
             printf("Invalid mailbox \n");
             // Start receving messages 
             break;

          } else if(retVal == -1) {
             printf("Deposit failed rx not available\n");
          } 
      }
      
      retVal = destroy_mailbox(res);
      if(retVal == -1){
          printf( "SYSTEM CALL FAIL: Failed to delete mail box\n" );
     }else {
	   printf("Success: delete mail box  returned  %d \n",retVal);
     }   

   return 0;
}
