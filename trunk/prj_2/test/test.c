#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
//#include "MultiCastLib.h"

int m_id ; 

int main( int argc, char** argv )
{
	 char option=-1;  
	 printf("\n\n==================================================== \n");
	 printf("CS551 Project 2 MailBox IPC \n");
	 printf("==================================================== \n");
	 while(1){
		printf("\nChoose one of below options \n\n");
		printf("1. Test Cases demonstrating Functionalities \n");
		printf("2. Test Cases demonstrating Error Conditions \n");
		printf("0. Exit \n\n");
		printf("Enter your option : ");
		scanf(" %c",&option);
		
		switch(option){
			 case '1' : tc_DemoFunctionalities();
				break;
			 case '2' : tc_DemoErrorConditions();
				break;			
			 case '0' : exit(0);
				break;
			 default : printf("Invalid Entry \n");
				break;
		}
	 }
}


int tc_DemoFunctionalities(){
	int rval1, i;
    char msg[200]={0};
	char mgrpName[200];
	int mgrpId;
	int mgrpIds[11];
    int rval2,rval3, rval4;
	int c;
	
	int option=-1;
	//resetAll();
	printf("\n\n==================================================== \n");
	printf("Test Cases demonstrating Functionalities \n");
	printf("==================================================== \n");
	while(1){
		printf("\nChoose one of below Test Cases \n\n");
		printf("1.  Create Mailbox \t2. Create Multiple Mailbox \t 3. Destroy Mailbox \n4.  Get Available Mailboxes\t5.  Send Mail \t6.  Blocking Sender \n7. Receive  Mail \t8. Ordering the Message\n"); 
		printf("0.  Back \n\n");

		printf ( "\n------ Please Enter option 7 in the other terminal to simulate Receiver First before selecting option 5-----\n");
   		
		printf("\nEnter your TestCase# : ");
		scanf("%d",&option);
		
		switch(option){
			 case 1 : tc_createMailbox();
				break;
			case 2 : tc_createMultipleMailbox();
				break;
			 case 3 : tc_destroyMailbox();
				break;			
			 case 4 : tc_getAvailableMailbox();
				break;
			 case 5 : tc_deposit();
				break;
			 case 6 : tc_block_deposit();
				break;
			 case 7 : tc_Receive_mail();
				break;
			 case 8 : tc_message_ordering();
				break;
				break;
			case 0 : return 0;
				break;
			 default : printf("Invalid Entry \n");
				break;
		}
		printf("\nHit 1 to execute new test case! \n");
		scanf(" %c",&c); 		
	 }
}

/***************************************************************************************
 ************************  Demo Functionalities ****************************************
 **************************************************************************************/
int tc_createMailbox(){
   int mb_id; 
   printf ( "\n\n ------ Creating mail box with Group permission------\n");
   mb_id = create_mailbox(0x2);
    if(mb_id == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with group permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with group permission  created: MB ID = %d ",mb_id);
   }
   destroy_mailbox(mb_id);
   
   return ; 
}

/*
int tc_createMailboxWithoutDelete(){
   
   printf ( "\n\n ------ Creating mail box with Group permission------\n");
   m_id = create_mailbox(0x2);
    if(m_id == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with group permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with group permission  created: MB ID = %d ",m_id);
   }
   //destroy_mailbox(mb_id);
   
   return ; 
}

*/

int tc_createMultipleMailbox(){

   int mb_id1, mb_id2, mb_id3;
   
   printf ( "\n\n ------ Creating mail box with Self permission------\n");
   mb_id1 = create_mailbox(0x4);
    if(mb_id1 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with Self permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with Self permission  created: MB ID = %d ",mb_id1);
   }
   
   printf ( "\n\n ------ Creating mail box with Group permission------\n");
   mb_id2 = create_mailbox(0x2);
    if(mb_id2 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with group permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with group permission  created: MB ID = %d ",mb_id2);
   }
   
   printf ( "\n\n ------ Creating mail box with Everybody permission------\n");
   mb_id3 = create_mailbox(0x1);
    if(mb_id3 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with Everybody permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with Everybody permission  created: MB ID = %d ",mb_id3);
   }
   
   destroy_mailbox(mb_id1);
   destroy_mailbox(mb_id2);
   destroy_mailbox(mb_id3);
        
   return ; 
}


int tc_destroyMailbox(){
   int mb_id1, mb_id2, retVal;
   int * mb_list = NULL;
   int i;
   
   printf ( "\n\n ------ Creating mail box with Self permission------\n");
   mb_id1 = create_mailbox(0x4);
    if(mb_id1 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with Self permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with Self permission  created: MB ID = %d ",mb_id1);
   }
   
   
   printf ( "\n\n ------ Creating mail box with Group permission------\n");
   mb_id2 = create_mailbox(0x2);
    if(mb_id2 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with group permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with group permission  created: MB ID = %d ",mb_id2);
   }
   
   
   printf ( "\n\n------ Fetch List of Created Mailbox -----\n");
   mb_list = (int * ) malloc (12 *2*sizeof(int));
   retVal = get_av_mailboxes(mb_list);
  if(retVal == -1){
	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );
	   return -1;
   }else {
          printf("MailBox ID registered are\n" );
	 for ( i = 0 ; i < 2 ; i ++ ) {
		   printf("mb_ %d _id  = %d\n", i, *(mb_list + (2*i)));
        }
   }
   
     printf ( "\n\n------Deleting all the Mailbox ------\n");
      retVal = destroy_mailbox(*mb_list);
      if(retVal == -1){
          printf( "SYSTEM CALL FAIL: Failed to delete mail box %d\n", *mb_list );
     }else {
	   printf("Mailbox ID %d deleted \n",*mb_list);
     }  
   
    retVal = destroy_mailbox(*(mb_list + 2));
      if(retVal == -1){
          printf( "SYSTEM CALL FAIL: Failed to delete mail box %d\n", *(mb_list + 2) );
     }else {
	   printf("Mailbox ID %d deleted \n", *(mb_list + 2));
     } 
     
     free ( mb_list);
}

				
int tc_getAvailableMailbox(){	
   
   int mb_id1, mb_id2,mb_id3, mb_id4, mb_id5,retVal;
   int * mb_list = NULL;
   int i;
   
   printf ( "\n\n ------ Creating mail box with Self permission------\n");
   mb_id1 = create_mailbox(0x4);
    if(mb_id1 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with Self permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with Self permission  created: MB ID = %d ",mb_id1);
   }
   
   
   printf ( "\n\n ------ Creating mail box with Group permission------\n");
   mb_id2 = create_mailbox(0x2);
    if(mb_id2 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with group permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with group permission  created: MB ID = %d ",mb_id2);
   }
   
   printf ( "\n\n ------ Creating mail box with Self permission------\n");
   mb_id3 = create_mailbox(0x4);
    if(mb_id3 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with Self permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with Self permission  created: MB ID = %d ",mb_id3);
   }
   
   printf ( "\n\n ------ Creating mail box with Self permission------\n");
   mb_id4 = create_mailbox(0x4);
    if(mb_id4 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with Self permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with Self permission  created: MB ID = %d ",mb_id4);
   }
   
   printf ( "\n\n ------ Creating mail box with Group permission------\n");
   mb_id5 = create_mailbox(0x2);
    if(mb_id5 == -1){
	   printf( "SYSTEM CALL FAIL: Failed to create mail box with group permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box with group permission  created: MB ID = %d ",mb_id5);
   }
   
   
   printf ( "\n\n------ Fetch List of Created Mailbox -----\n");
   mb_list = (int * ) malloc (12 *2*sizeof(int));
   retVal = get_av_mailboxes(mb_list);
  if(retVal == -1){
	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );
	   return -1;
   }else {
          printf("MailBox ID registered are\n" );
	 for ( i = 0 ; i < 5 ; i ++ ) {
		   printf("mb_ %d _id  = %d\n", i, *(mb_list + (2*i)));
        }
   }
   
   destroy_mailbox(*mb_list);
   destroy_mailbox(*(mb_list + 2) );
   destroy_mailbox(*(mb_list + 4) );
   destroy_mailbox(*(mb_list + 6) );
   destroy_mailbox(*(mb_list + 8) );
          
     free ( mb_list);
}

 
int tc_deposit(){

   int res, retVal, mb_id ;
   int * mb_list = NULL;
   int *m_pid = NULL;
   char* msg;
    int *destin;
   int i = 0;
   
  printf ( "\n\n------Fetching Available Mailbox----\n");
  mb_list = (int * ) malloc (12 *2*sizeof(int));
  retVal = get_av_mailboxes(mb_list);
  if(retVal == -1){
	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );
	   return -1;
   }else {
          printf("Available Mailbox are : \n" );
	 for ( i = 0 ; i < 1 ; i ++ ) {
		 mb_id = *(mb_list + (2*i)); 
		 m_pid = *(mb_list + (2*i)+1);
		 printf("mb_id  = %d\n",  *(mb_list + (2*i)));
        }
   }
   
   printf ( "\n\n------Register Sender to the mailbox  %d------\n", mb_id);
      retVal = register_mb(mb_id);
      if(retVal == -1){
          printf( "FAIL: Failed to register mail box\n" );
	  return -1;
     }else if(retVal == 0){
           printf("Success: Registered to the Mailbox \n");
     }else if(retVal == -2){
           printf("Success: Sender already Registered \n");
     }
     
       
    
    printf("\nDeposting Message to Mailbox \n");
    
    destin = (int *) malloc(40*sizeof(int));
    destin[0] = mb_list[1];
    destin[1] = -1;
    msg = (char * ) malloc(256);
    strcpy(msg,"Hellow World");
    retVal = deposit(destin,msg);
   return 0;
   
}

			
			 
int tc_block_deposit(){
	return 1;
}


int tc_Receive_mail(){

   int res1, retVal ;
   char * msg;
   int m_id;
   
   printf ( " ---------Creating Mailbox with All permission------\n");
   m_id = create_mailbox(0x7);
   if(m_id == -1){
           printf( "SYSTEM CALL FAIL: Failed to create mail box with All permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box  Created with All permission : ID = %d ",m_id);
   }
   
  msg = (char * ) malloc(256);
  memset ( msg, '#', 256);
  printf("Retrieving message from the mailbox");
  res1 = retrieve(1,msg);
  if(res1 == -1 ) {
      printf("Retrieve failed returned %d \n",res1);
  }else{
      printf("Retrieve success : Msg Received is : \n");
      while ( *msg != '#'){
        printf("%c",*msg);
	msg++;
      }
     printf("\n");
     
       
  }
  retVal = destroy_mailbox(m_id);
     if(retVal == -1){
        printf( "SYSTEM CALL FAIL: Failed to delete mail box\n" );
     }else {
        printf("Success: Deleted the mailbox\n");
     }
   return 0;
}


			
int tc_message_ordering(){ 
}




int tc_DemoErrorConditions(){
    int rval1, i;
    char msg[200]={0};
    char mgrpName[200];
    int mgrpId;
    int mgrpIds[11];
    int rval2,rval3, rval4;
    int c;
	
	int option=-1;
	//resetAll();
	printf("\n\n==================================================== \n");
	printf("Test Cases demonstrating Error Conditions \n");
	printf("==================================================== \n");
	while(1){
		printf("\nChoose one of below Test Cases \n\n");
		printf("9.  Create more than Maximum Mailbox \n");
		printf("10. Get List of Available Mailbox - before creating any mailbox \n");
		printf("11. Register to Mailbox - Invalid Mailbox ID \n");
		printf("12. Register to Mailbox - User already Registered.\n");
		printf("13. Deposit message to mailbox - Invalid Destination PID\n");
	        printf("14. Deposit message to mailbox - Mailbox is full ( Blocking Sender ) \n");
		printf("15. Retrieve message - Mailbox Empty ( blocking Reveiver ) \n");
	        printf("0.  Back \n\n");
		printf("Enter your TestCase# : ");
		scanf("%d",&option);
				
		switch(option){
			 case 9 : tc_err_create_mailbox_more_than_maxLimit();
				break;
			 case 10 : tc_errget_av_mailbox();
				break;
			 case 11 : tc_err_register_to_invalid_mailbox_id();
				break;
			 case 12 : tc_err_register_to_mailbox_user_already_registered(); 
				break;
			 case 13 : tc_err_deposit_msg_invalid_dest_id(); 
				break;
			// case 14 : tc_err_deposit_msg_mailbox_full();
				break;
  			 //case 15 : tc_err_retrieve_msg_mailbox_empty();
				break;
			 case 0 : return 0;
				break;
			 default : printf("Invalid Entry \n");
				break;
		}
		printf("\nHit 1 to execute new test case!\n");
		scanf(" %c",&c); 		
	 }
}

 
/***************************************************************************************
 ************************  Demo Error Conditions ***************************************
 **************************************************************************************/
 int tc_err_create_mailbox_more_than_maxLimit() 
 {
	 int mb_id[10], mb_id_err;
	 int i;
	 
	 for ( i =0; i <10; i++) 
	 {
		mb_id[i] = create_mailbox(0x2);
		 if(mb_id[i] == -1){
		   printf( "SYSTEM CALL FAIL: Failed to create mail box with group permission \n" );
		   return -1;
		 }else {
		    printf("Success: Mail box created: MB ID = %d \n",mb_id[i]);
		 }
	 }
	 
	 mb_id_err= create_mailbox(0x2);
	 if(mb_id_err == -1){
            printf( "FAIL: Mail Box FULL \n" );
	 }else {
	    printf("Success: Mail box created: MB ID = %d \n",mb_id_err);
	}
	 
	 for ( i =0; i <10; i++) 
	 {
		destroy_mailbox(mb_id[i]);
	 }
	 
    
	 return 1;
 }

int tc_errget_av_mailbox()
{
   int retVal;
   int * mb_list = NULL;
   mb_list = (int * ) malloc (12 *2*sizeof(int));
   memset ( mb_list, -1, 2 *2*sizeof(int));
   
   get_av_mailboxes(mb_list);  
   if ( *mb_list == -1) {
	   printf ( " There is no mailbox created \n");
   }   
   	return 1;
}


int tc_err_register_to_invalid_mailbox_id()
{
   int mb_id; 
   int retVal;
   int * mb_list = NULL;
   int *m_pid = NULL;
   int i;
   
   printf ( "\n\n ------ Creating mail box ------\n");
   mb_id = create_mailbox(0x2);
    if(mb_id == -1){
	   printf( "Failed to create mail box \n" );
	   return -1;
   }else {
          printf("Success: Mail box created: MB ID = %d ",mb_id);
   }
   
   
  printf ( "\n\n------Fetching Available Mailbox----\n");
  mb_list = (int * ) malloc (12 *2*sizeof(int));
  retVal = get_av_mailboxes(mb_list);
  if(retVal == -1){
	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );
	   //return -1;
   }else {
          printf("Available Mailbox are : \n" );
	 for ( i = 0 ; i < 1 ; i ++ ) {
		 mb_id = *(mb_list + (2*i)); 
		 m_pid = *(mb_list + (2*i)+1);
		 printf("mb_id  = %d\n",  *(mb_list + (2*i)));
        }
   }
   
   printf ( "\n\n------Register Sender to the mailbox  5------\n");
      retVal = register_mb(5);
      if(retVal == -1){
          printf( "Invalid Mailbox ID. Failed to register mail box\n" );
	  return -1;
     }else if(retVal == 0){
           printf("Success: Registered to the Mailbox \n");
     }else if(retVal == -2){
           printf("Success: Sender already Registered \n");
     }
     
     destroy_mailbox(mb_id);

	return 1;
}


int tc_err_register_to_mailbox_user_already_registered()
{
   int mb_id; 
   int retVal;
   int * mb_list = NULL;
   int *m_pid = NULL;
   int i;
   
   printf ( "\n\n ------ Creating mail box ------\n");
   mb_id = create_mailbox(0x2);
    if(mb_id == -1){
	   printf( "Failed to create mail box \n" );
	   return -1;
   }else {
          printf("Success: Mail box created: MB ID = %d ",mb_id);
   }
   
   
  printf ( "\n\n------Fetching Available Mailbox----\n");
  mb_list = (int * ) malloc (12 *2*sizeof(int));
  retVal = get_av_mailboxes(mb_list);
  if(retVal == -1){
	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );
	   //return -1;
   }else {
          printf("Available Mailbox are : \n" );
	 for ( i = 0 ; i < 1 ; i ++ ) {
		 mb_id = *(mb_list + (2*i)); 
		 m_pid = *(mb_list + (2*i)+1);
		 printf("mb_id  = %d\n",  *(mb_list + (2*i)));
        }
   }
   
   printf ( "\n\n------Register Sender to the mailbox  %d------\n", mb_id);
      retVal = register_mb(mb_id);
      if(retVal == -1){
          printf( "Invalid Mailbox ID. Failed to register mail box\n" );
	  return -1;
     }else if(retVal == 0){
           printf("Success: Registered to the Mailbox \n");
     }else if(retVal == -2){
           printf("Success: Sender already Registered \n");
     }
     
     
   printf ( "\n\n------Register Sender again to the same mailbox  %d------\n", mb_id);
      retVal = register_mb(mb_id);
      if(retVal == 0){
           printf("Success: Registered to the Mailbox \n");
     }else {
           printf(" Sender already Registered \n");
     }
     
    destroy_mailbox(mb_id);
    return 1;
}


int tc_err_deposit_msg_invalid_dest_id()
{
   int mb_id; 
   int retVal;
   int * mb_list = NULL;
   int *m_pid = NULL;
   int *destin = NULL;
   char * msg = NULL;
   int i;
   
   printf ( "\n\n ------ Creating mail box ------\n");
   mb_id = create_mailbox(0x2);
    if(mb_id == -1){
	   printf( "Failed to create mail box \n" );
	   return -1;
   }else {
          printf("Success: Mail box created: MB ID = %d ",mb_id);
   }
   
   
  printf ( "\n\n------Fetching Available Mailbox----\n");
  mb_list = (int * ) malloc (12 *2*sizeof(int));
  retVal = get_av_mailboxes(mb_list);
  if(retVal == -1){
	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );
	   //return -1;
   }else {
          printf("Available Mailbox are : \n" );
	 for ( i = 0 ; i < 1 ; i ++ ) {
		 mb_id = *(mb_list + (2*i)); 
		 m_pid = *(mb_list + (2*i)+1);
		 printf("mb_id  = %d\n",  *(mb_list + (2*i)));
        }
   }
   
   printf ( "\n\n------Register Sender to the mailbox  %d------\n", mb_id);
      retVal = register_mb(mb_id);
      if(retVal == -1){
          printf( "Invalid Mailbox ID. Failed to register mail box\n" );
	  return -1;
     }else if(retVal == 0){
           printf("Success: Registered to the Mailbox \n");
     }else if(retVal == -2){
           printf("Success: Sender already Registered \n");
     }
     
           
    
    printf("\nDeposting Message to Mailbox \n");
    
    destin = (int *) malloc(40*sizeof(int));
    destin[0] = 5;
    destin[1] = -1;
    msg = (char * ) malloc(256);
    strcpy(msg,"Hellow World");
    retVal = deposit(destin,msg);
    if ( retVal == 0 ) {
       printf ("\nInvalid Destination ID, Failed to send\n");
    }
    destroy_mailbox(mb_id);

	return 1;
}



/*
int tc_err_deposit_msg_mailbox_full()
{

   int mb_id; 
   int retVal;
   int * mb_list = NULL;
   int *m_pid = NULL;
   int *destin = NULL;
   char * msg = NULL;
   int i;
   
   printf ( "\n\n ------ Creating mail box ------\n");
   mb_id = create_mailbox(0x2);
    if(mb_id == -1){
	   printf( "Failed to create mail box \n" );
	   return -1;
   }else {
          printf("Success: Mail box created: MB ID = %d ",mb_id);
   }
   
   
  printf ( "\n\n------Fetching Available Mailbox----\n");
  mb_list = (int * ) malloc (12 *2*sizeof(int));
  retVal = get_av_mailboxes(mb_list);
  if(retVal == -1){
	   printf( "SYSTEM CALL FAIL: get_av_mailboxes FAILED \n" );
	   //return -1;
   }else {
          printf("Available Mailbox are : \n" );
	 for ( i = 0 ; i < 1 ; i ++ ) {
		 mb_id = *(mb_list + (2*i)); 
		 m_pid = *(mb_list + (2*i)+1);
		 printf("mb_id  = %d\n",  *(mb_list + (2*i)));
        }
   }
   
   printf ( "\n\n------Register Sender to the mailbox  %d------\n", mb_id);
      retVal = register_mb(mb_id);
      if(retVal == -1){
          printf( "Invalid Mailbox ID. Failed to register mail box\n" );
	  return -1;
     }else if(retVal == 0){
           printf("Success: Registered to the Mailbox \n");
     }else if(retVal == -2){
           printf("Success: Sender already Registered \n");
     }
     
           
    
    printf("\nDeposting Message to Mailbox \n");
    
    destin = (int *) malloc(40*sizeof(int));
    destin[0] = mb_list[1];
    destin[1] = -1;
    msg = (char * ) malloc(256);
    strcpy(msg,"Hellow World");
    retVal = deposit(destin,msg);
    if ( retVal == 0 ) {
       printf ("\nInvalid Destination ID, Failed to send\n");
    }else {
       printf ( "Msg Successfully sent\n");
    }
    free (destin);
    
    destroy_mailbox(mb_id);

	return 1;
}



int tc_err_retrieve_msg_mailbox_empty()
{

   int mb_id;
   int res1, retVal ;
   char * msg;
   
   printf ( " ---------Creating Mailbox ------\n");
   mb_id = create_mailbox(0x7);
   if(mb_id == -1){
           printf( "SYSTEM CALL FAIL: Failed to create mail box with All permission \n" );
	   return -1;
   }else {
          printf("Success: Mail box  Created: ID = %d\n ",mb_id);
   }
   
  msg = (char * ) malloc(256);
  memset ( msg, '#', 256);
  printf("Retrieving message from the mailbox\n");
  res1 = retrieve(1,msg);
  printf ( " Res1 = %d\n", res1);
  if(res1 != 0 ) {
      printf("Mailbox Empty: Retrieve failed \n");
  }else{
      printf("Retrieve success : Msg Received is : \n");
      while ( *msg != '#'){
        printf("%c",*msg);
	msg++;
      }
     printf("\n");
  }
  
  retVal = destroy_mailbox(mb_id);
     if(retVal == -1){
        printf( "SYSTEM CALL FAIL: Failed to delete mail box\n" );
     }else {
        printf("Success: Deleted the mailbox\n");
     }
   
	return 1;
}
*/
			