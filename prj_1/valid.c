//----------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementatiom of Operating Systems
// Fall 2012 , Group 3 
// 
//-Description   -------------------------------------------
// This file validates the command entered by the user.
//----------------------------------------------------------


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>

#define MAX_PATH_LEN 4096

enum {
    
    INVALID_COMMAND = -1,
    NULL_COMMAND,
    VALID_COMMAND
};

/* Function: validateIfElseCondition
* Brief : Validates "IF cmd then cmd else cmd if" Command.
*/

int validateIfElseCondition(char **listOfCommands, int numArguments) {
    //Local Enum to simulate the stack operation for validating if .. Then .. Else command 
    enum {
        IF = 1,
        THEN,
        ELSE,
        FI
    } ;
    
    int i, j;
    int valid = 1;
    int prev = FI; 
    int stk[1000];
    int top = -1;
    if ( debug_en ) printf ( " Entered validateIfElseCondition function\n");
    for ( i = 0; i < numArguments; i++){
        if ( strcmp(listOfCommands[i], "if") == 0 ) {
            // The IF condition enclosed validates that "IF" has to be the first symbol or Nested IF should be preceeded only by "IF" or "THEN" or "ELSE"
            if (!( i == 0 || (( strcmp(listOfCommands[i -1], "if") == 0 ) || ( strcmp(listOfCommands[i -1], "then") == 0 ) || ( strcmp(listOfCommands[i -1], "else") == 0 )) )){
                if ( debug_en ) printf ( " validation of IF symbol failed \n");
                return INVALID_COMMAND;
            }
            stk[++top] = IF;
        }else if ( strcmp(listOfCommands[i], "then") == 0 ) {
            //The symbol precedding "THEN" can be either a command or "fi" else its a INVALID Command
            if (( strcmp(listOfCommands[i -1], "if") == 0 ) || ( strcmp(listOfCommands[i -1], "then") == 0 ) || ( strcmp(listOfCommands[i -1], "else") == 0 )) {
                if ( debug_en ) printf ( " validation of THEN symbol failed \n");
                return INVALID_COMMAND;
            }
            stk[++top] = THEN;
        }else if ( strcmp(listOfCommands[i], "else") == 0 ) {
            //The symbol precedding "ELSE" can be either a command or "fi" else its a INVALID Command
            if (( strcmp(listOfCommands[i -1], "if") == 0 ) || ( strcmp(listOfCommands[i -1], "then") == 0 ) || ( strcmp(listOfCommands[i -1], "else") == 0 )) {
                if ( debug_en ) printf ( " validation of ELSE symbol failed \n");
                return INVALID_COMMAND;
            }
            stk[++top] = ELSE;
        }else if ( ( strcmp(listOfCommands[i], "fi") == 0 )){
            //The symbol precedding "fi" can be either a command or "fi" else its a INVALID Command
            if (( strcmp(listOfCommands[i -1], "if") == 0 ) || ( strcmp(listOfCommands[i -1], "then") == 0 ) || ( strcmp(listOfCommands[i -1], "else") == 0 )) {
                if ( debug_en ) printf ( " validation of FI symbol failed \n");
                return INVALID_COMMAND;
            }
            //IF "FI" is encountered then pop all the symbols pushed into the stack until matching "IF" is found
            //While popping out, make sure the order of popping is ELSE, THEN, IF. IF the order changes then the 
            // command entered is Invalid.
            for (j = 1; top >= 0 && j <= 3; j++ ) {
                if ((stk[top] == ELSE) && (prev == FI)){
                    prev = ELSE;
                    top--;
                }else if ((stk[top] == THEN) && (prev == ELSE)){
                    prev = THEN;
                    top--;
                }else if ((stk[top] == IF) && (prev == THEN)){
                    prev = FI;	
                    top--;
                }else{
                    valid = 0;
                    break;
                }
            }
            if ( !valid){
                if (debug_en) printf ( "Validation of IF Else failed \n");
                return INVALID_COMMAND;
            }
        }
    }
    if ( top >= 0 ) 
    return INVALID_COMMAND;
    else
    return VALID_COMMAND;
}

/* Function: validateCommand
* Brief : Validates the command entered by the user.
*/

int validateCommand(char ***cmd_list, int numArguments)
{
    if ( debug_en ) printf ( " Entered ValidateCommand function\n");
    int found = 0;
    char *cmd, *tempCmd;
    char binPath[MAX_PATH_LEN];
    char usrBinPath[MAX_PATH_LEN];
    int i , j;
    char ** listOfCommands;
    char *alias_val;
    listOfCommands = *cmd_list;	
    if( numArguments <= 0) {
        if(debug_en) printf("Null command\n");
        return NULL_COMMAND;
    }
    if(debug_en) printf("Validation input=%s=\n",listOfCommands[0]);	
    for ( i = 0; i < numArguments; i++){
        
        tempCmd = strdup(listOfCommands[i]);
        cmd = strtok(tempCmd, " ");
        // Entered command does not have any options at all.
        if(NULL == cmd) {
            cmd = tempCmd;
        }
        
        if(debug_en) printf("Searching %s for aliasing \n",cmd);	
        alias_val = get(&alias_s,cmd);
        if(debug_en) printf("alias_val : %s = %s \n",cmd,alias_val); 
        if(alias_val != NULL) {
            int cmd_len ;
            char *str;
            tempCmd = strdup(listOfCommands[i]);
            int len = strlen(usrBinPath) + strlen(tempCmd) + 2; 
            str = (char *) malloc(len);
            strcpy(str,alias_val);
            //strcat(listOfCommands[0]," ");
            for(len=0;len<strlen(tempCmd);len++){
                if(tempCmd[len] == 0x20) 
                break;
            }
            for(cmd_len = strlen(str),j=0;j<strlen(tempCmd) -len ;j++){
                *(str+cmd_len+j) = tempCmd[len+j];
                //printf("Assigning %c to %dth post",tempCmd[len+i],cmd_len+i);
            }
            *(str+cmd_len+j) = 0x0;
            // printf("str of space = %d\n",cmd_len+i);
            if(debug_en) printf("\n list of command = %s \n",str);
            listOfCommands[i] = str;
        } else {
            if(debug_en) printf("Search alias failed \n");
        }
        
        //Gets the command ignoring options
        tempCmd = strdup(listOfCommands[i]);
        cmd = strtok(tempCmd, " ");
        // Entered command does not have any options at all.
        if(NULL == cmd) {
            cmd = tempCmd;
        }
        
        // This loop is executed only once when the encountered command is IF
        if(0 == strcmp(cmd, "if") && (i == 0)){
            if ( INVALID_COMMAND == validateIfElseCondition(listOfCommands, numArguments)) {
                if ( debug_en ) printf ( " Validation of IF Else command failed\n");
                return INVALID_COMMAND;
            }
        }
        else { 
            //Creates command path
            sprintf(binPath, "%s/%s", "/bin", cmd);
            sprintf(usrBinPath, "%s/%s", "/usr/bin", cmd);
            
            // Checks command existence
            if (((strcmp(cmd, "set") == 0) || (strcmp(cmd, "if") == 0) || (strcmp(cmd, "then") == 0) ||(strcmp(cmd, "else") == 0) ||
            (strcmp(cmd, "fi") == 0) || (strcmp(cmd, "exit") == 0) || (cmd[0] ==  '.') || (cmd[0] ==  '$') || (strcmp(cmd, "var") == 0) ||
            (strcmp(cmd, "print") == 0) || (strcmp(cmd, "alias") == 0) || (strncmp(cmd, "unalias", 7) == 0) ||
            (strcmp(cmd, "expr") == 0) || (strcmp(cmd, "cd") == 0) || (strcmp(cmd,"hist") == 0) || (access(binPath, F_OK) >= 0) ||
            (access(usrBinPath, F_OK) >= 0))) {
                
                if (debug_en ) printf ( " Command %s exist\n ", cmd);	
                found = 1;
                if((strcmp(cmd,"alias")==0) || (strcmp(cmd,"set")==0)) {
                    if((listOfCommands[i+1] != NULL ) && (listOfCommands[i+2] != NULL)) {
                        return VALID_COMMAND;
                    }
                }else if(strcmp(cmd,"cd")==0 || strncmp(cmd,"cd ",3)==0){
                    return VALID_COMMAND;
                }
                
                if( (access(binPath, F_OK) >= 0) || (access(usrBinPath, F_OK) >= 0)){
                    int cmd_len ;
                    char *str;
                    tempCmd = strdup(listOfCommands[i]);
                    int len = strlen(usrBinPath) + strlen(tempCmd) + 2; 
                    str = (char *) malloc(len);
                    if(access(binPath, F_OK) >= 0) {
                        strcpy(str,binPath);
                    }else{
                        strcpy(str,usrBinPath);
                    }
                    //strcat(listOfCommands[0]," ");
                    for(len=0;len<strlen(tempCmd);len++){
                        if(tempCmd[len] == 0x20) 
                        break;
                    }
                    for(cmd_len = strlen(str),j=0;j<strlen(tempCmd) -len ;j++){
                        *(str+cmd_len+j) = tempCmd[len+j];
                        //printf("Assigning %c to %dth post",tempCmd[len+i],cmd_len+i);
                    }
                    *(str+cmd_len+j) = 0x0;
                    // printf("str of space = %d\n",cmd_len+i);
                    // printf("\n list of command = %s \n",str);
                    listOfCommands[i] = str;
                }
                
            } else {
                if ( debug_en ) printf ( "Command %s does Not exist\n", cmd);
                found = 0;
            }
            if ( !found ) {
                if ( debug_en ) printf ("Checking for special commands %s \n", listOfCommands[i]);
                if(('>' == listOfCommands[i][0]) || ('|' == listOfCommands[i][0]) || strcmp("=", listOfCommands[i])==0) {
                    if ( debug_en ) printf ( "Special command %s found\n", listOfCommands[i]);
                    found = 1;
                    if((i == numArguments - 1)  || ( i == 0 )) {
                        if(debug_en) printf("Invalid command.. TOO less argument\n");
                        return INVALID_COMMAND;
                    }
                    if ((listOfCommands[i + 1][0] == '&') || (listOfCommands[i + 1][0] == '>')
                    || (listOfCommands[i + 1][0] == '|') || (listOfCommands[i+ 1][0] == '=')){
                        
                        if(debug_en) printf("Invalid command ... \n");
                        return INVALID_COMMAND;
                    }
                    if  ('=' == listOfCommands[i][0]) {
                        if ((strcmp(listOfCommands[i-1], "alias") ==0) || ( strcmp(listOfCommands[i-1], "set") ==0)) {
                            if(debug_en) printf ( "Invalid command Alias/Set \n");
                            return INVALID_COMMAND;
                        }
                    }
                    if ( ('>' == listOfCommands[i][0]) || ('=' == listOfCommands[i][0])){
                        i++;
                    }
                } else if(('&' == listOfCommands[i][0])){
                    found = 1;
                    if(i !=  numArguments - 1) {
                        if(debug_en) printf("Invalid command\n");
                        return INVALID_COMMAND;
                    }
                }
            }
        } // end of flag if
    }// end of else
    if ( found ){
        
        return VALID_COMMAND;
        *cmd_list = listOfCommands;
    }else
    return INVALID_COMMAND;
}

