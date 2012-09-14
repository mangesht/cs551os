#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define WHITESPACE      ' '
#define PARAM_SIZE 30


int parsecmd( char *, char ***);
void handleException(int );
void cleanCmd(char *, int);
char * substr(char *, int , int );


int parseCmd(char * cmd, char *** cmd_list){

  int i,j=0,char_count=0, space_encountered=0;

  // handles cleaning the input command
  cleanCmd(cmd, strlen(cmd));
  char ** cmd_params = *cmd_list;
  // hanldes alias command
  if ( strstr(cmd,"alias ") || strstr(cmd,"set ")){ 
	 
	char *ptr = strstr(cmd,"alias ");
	if (ptr == NULL ){
		ptr = strstr(cmd,"set ");
	 }
	
	if (ptr != NULL && ptr==cmd ){
	   char * pch = (char*)strtok (ptr," =");
	   while (pch != NULL){	  
		  cmd_params[j++]= pch;
		  pch = strtok (NULL, "=");

		  if (pch!=NULL){
		  	char * s = strstr(pch,"\"");
			if (s != NULL){
			  pch=pch+1;
			  s = strstr(pch,"\"");
			  if (s != NULL )
				 pch[strlen(pch)-1]='\0';
				}
		   }
		}
	 return 1;	
	 }
  }


  char * mem =  (char*)malloc(sizeof(char)* 100);
  if (mem == NULL )  handleException(1);
  char * param = mem;

  char *pLoc;
  for (i=0;i<strlen(cmd)+1;i++){
   if ( (cmd[i]=='&' || cmd[i]=='>' || cmd[i]=='|' || cmd[i]=='\0' )&& char_count != 0 ){
   	   *mem='\0';
	   cmd_params[j++]= param;
	   if (cmd[i] != '\0'){
	       mem =  (char*)malloc(sizeof(char)* 2);
	       param=mem;
	       *mem++ = cmd[i];
		   *mem = '\0';
	       cmd_params[j++]= param;

	       mem =  (char*)malloc(sizeof(char)* PARAM_SIZE);
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
        
      pLoc=strstr(param,"if ");
      if( pLoc!=NULL) {	  	  
	      char * temp = (char*)malloc(sizeof(char)* 3);
	  	  temp="if";	  
		  cmd_params[j++]= substr(param,0, pLoc-param);
  		  cmd_params[j++]= temp;	
	 	  mem =  (char*)malloc(sizeof(char)* PARAM_SIZE);
	      param=mem;			
	  }
	  
	  pLoc=strstr(param," then ");
      if( pLoc!=NULL) {
	  	  char * temp = (char*)malloc(sizeof(char)* 7);
	  	  temp="then";
	 	  cmd_params[j++]= substr(param,0, pLoc-param);	  
		  cmd_params[j++]= temp;
	 	  mem =  (char*)malloc(sizeof(char)* PARAM_SIZE);
	      param=mem;
	  }
	  

	  
	  pLoc=strstr(param," else ");
      if( pLoc!=NULL) {
	      char * temp = (char*)malloc(sizeof(char)* 7);
	  	  temp="else";	  
	  	  cmd_params[j++]=  substr(param,0, pLoc-param);
  	      cmd_params[j++]= temp;
	 	  mem =  (char*)malloc(sizeof(char)* PARAM_SIZE);
	      param=mem;
	  }
	  
	  pLoc=strstr(param," fi");
      if( pLoc!=NULL) {
	  	  char * temp = (char*)malloc(sizeof(char)* 3);
	  	  temp="fi";	  
	 	  cmd_params[j++]=  substr(param,0, pLoc-param);
   	      cmd_params[j++]= temp;
	 	  mem =  (char*)malloc(sizeof(char)* PARAM_SIZE);
	      param=mem;	    
	  }
	  }
   }
return 1 ;
}


void handleException(int code){
  switch (code){
 	case 1 : printf(" Out of Memory !!!");
    	      exit(0);
  }
}



void cleanCmd(char *pCmd, int n){
    size_t i;
    char *p = pCmd;

	for(i = 0; i < n; i++)
	    if('\t' == pCmd[i])  pCmd[i] = ' ';

    /* remove trailing space */
    for(i = n-1; i >= 0; i--)
        if(' ' == pCmd[i])   pCmd[i] = '\0';
        else  break;

    /* remove leading space */
    for(i = 0; i < n; i++)
        if(' ' == pCmd[i]) p = pCmd + i + 1;
        else break;
 		pCmd=p;
	return;
}

/* Extracts substring from the given string 								*/
/* Input:  	 pstr - source string											*/
/*  		 start - start location  string									*/
/*  		 numchars - no of characters to extract from start location		*/
char * substr(char *pstr, int start, int numchars){
	char *pnew = malloc(numchars+1);
	strncpy(pnew, pstr + start, numchars);
	pnew[numchars] = '\0';
	return pnew;
}


