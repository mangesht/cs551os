//----------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementation of Operating Systems
// Fall 2012 , Group 3 
// 
//-Description   -------------------------------------------
// This is parse program, it parses the incoming single string and splits into 
// mutliple string 
//----------------------------------------------------------


void cleanCmd(char *pCmd, int n)
{
    int i;
    char *p = pCmd;

	for(i = 0; i < n; i++)
	    if('\t' == pCmd[i])  pCmd[i] = ' ';

    /* terminate the trailing spaces */
    for(i = n-1; i >= 0; i--)
        if(' ' == pCmd[i])   pCmd[i] = '\0';	
        else  break;

    /* remove leading spaces */
    for(i = 0; i < n; i++)
        if(' ' == pCmd[i]) p = pCmd + i + 1;
        else break;

 pCmd=p;
}



parseCmd(char * cmd, char ** cmdParams){
               
  int i,j=0,char_count=0 , new_param=0,space_encountered=0;
  char *mem =  (char*)malloc(sizeof(char)* 100);
  if (mem == NULL )  handleException(1);
  char *param = mem;	


  // handle cleaning the command
  cleanCmd(cmd, strlen(cmd));
 
 
  // hanlde alias 
  char * ptr = strstr(cmd,"alias");
  if (ptr != NULL){ 
	    char * pch;
	    int i=0;
	    pch = (char*)strtok (ptr," =");
		while (pch != NULL){
		  cmdParams[j++]= pch;
		  pch = strtok (NULL, " =");
		  
		  if (pch!=NULL){
		  	  char *s = strstr(pch,"\"");
			  if (s != NULL){ 
			  	 	pch=pch+1;
					*s = strstr(pch,"\"");
					if (s != NULL )
					pch[strlen(pch)-1]='\0';}
		  }			 
		}     
		return cmdParams; 	 	
  }
  
  // handle exit
  ptr = strstr(cmd,"exit");
  if (ptr != NULL){ 
		cmdParams[j++]= "exit";
	    return cmdParams; 	 	
  }
   

 for (i=0;i<strlen(cmd)+1;i++){
   if ( (cmd[i]=='&' || cmd[i]=='>' || cmd[i]=='|' || cmd[i]=='\0' )&& char_count != 0 ){          
   	   *mem='\0';
	   cmdParams[j++]= param;
	   
	   if (cmd[i] != '\0'){
	       mem =  (char*)malloc(sizeof(char)* 2);   
	       param=mem;
	       *mem++ = cmd[i]; 
		   *mem = '\0';
	       cmdParams[j++]= param;
	        
	       mem =  (char*)malloc(sizeof(char)* 50);   
	       param=mem; 
	       char_count=0;
		   }
     }
   else{
   		
   		// skips multiple spaces . Checks multiple occurence of space char. If second or more occurence then skips. 
   	    if (cmd[i]== ' ' && space_encountered==1) continue;
   	    
   	    // if here then either space is beeing seen for first time or its a non space char.
		// reset space_encountered flagon seeing a non space character.
		if (space_encountered==1)  space_encountered=0;
		
		// Seeing space for first time. set flag. Add space to the command
		if (cmd[i]== ' ') space_encountered=1;
		    
		char_count++;
        *mem++ = cmd[i];
        *mem='\0';
         
  /*      if( strcmp(param,"if")==0) {
			  cmdParams[j++]= param;
		 	  mem =  (char*)malloc(sizeof(char)* 3);   
	          param=mem;
	        //  continue;
		   }
		   
		char * s=strstr(param," then "); 
        if( (s!=NULL) {
			  cmdParams[j++]= param;
		 	  printf("then found\n");
		 	  mem =  (char*)malloc(sizeof(char)* 5);   
	          param=mem;
			  // continue;	          
		   }
		   
		   
        if( strcmp(param," else ")!=NULL) {
			  cmdParams[j++]= param;
		 	  printf("else found\n");
		 	  mem =  (char*)malloc(sizeof(char)* 5);   
			  param=mem;
		   }  */
		   
		   
		   
	  }
   }   
  // cmdParams[j]='\0';
return cmdParams ;
}


handleException(int code){
 switch (code){
 	case 1 : printf(" Out of Memory !!!");
    	      exit(0);
 }	 
}

