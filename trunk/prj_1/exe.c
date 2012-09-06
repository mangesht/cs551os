//----------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementatiom of Operating Systems
// Fall 2012 , Group 3 
// 
//-Description   -------------------------------------------
// The function execute and execute single perform the system calls required to execute the user asked functions.
//----------------------------------------------------------
char ** split(char *str){
    char ** str_list;
    char *tstr;
    int i;
    int num_spaces;
    int len;
    int skip = 0 ; 
    int line_num = 0 ; 
    int idx=0;
    // Count number of spaces 
    // You should exclude the spaces found inside quote
    len = strlen(str);
    for(i=0;i<len;i++){
        if(str[i] == 0x22 ) {
            // This indicates quote "
            skip = skip == 1 ? 0 : 1 ; 
        }else if(skip == 0 && str[i] == 0x20){
            num_spaces++;
        }
    }
    str_list = (char **) malloc(num_spaces * sizeof(char *));
    skip = 0 ;
    *tstr = (char *) malloc(256);
    // Split the input string using space as delimeter
    for(i=0;i<len;i++){
         if(str[i] == 0x22 ) {
            // This indicates quote "
            skip = skip == 1 ? 0 : 1 ; 
        }else if(skip == 0 && str[i] == 0x20){
            *tstr+idx = NULL;
            *str_list+line_num = tstr;
            line_num++;
            idx = 0 ;
        }else{
            *tstr+idx = str[i];
            idx++;
        }
    }
    return str_list;
}
int execute_single(char * cmd){
    char **cmd_list;
    int ret_val=0;
    cmd_list = split(cmd);
    ret_val = execv(cmd_list[0],cmd_list,0);
    if(ret_val == -1){
        perror("execution error:");
    }
    return ret_val;
}
int execute(char ***cmd_list_ptr,int start_idx){
    char **cmd_list;
    int ret_val;
    int i;
    cmd_list = *cmd_list_ptr;
    
    if( strncmp(cmd_list[start_idx+1],">",1)==0 || strncmp(cmd_list[start_idx+1],"|",1)==0) { 
        if(strncmp(cmd_list[start_idx+1],">",1)==0){
            // Redirection operator
        }else if(strncmp(cmd_list[start_idx+1],"|",1)==0){
            // pipe operator
        }
        ret_val = execute_single(cmd_list[start_idx+2]);
    }else if(strncmp(cmd_list[start_idx],"if",2)==0) {
        // Process it as if then else fi
        ret_val = execute(&cmd_list,start_idx+1);
        if(ret_val == TRUE ){
            // Find then part , it must be at cmd_list[start_idx+2]
            ret_val = execute(&cmd_list,start_idx+3);
        }else{
            // execute the false part of if ()
            // search for else
            int else_locn = -1;
            int fi_locn = -1;
            //for(i=start_idx+1;strncmp(cmd_list[i],"fi
            i = start_idx + 1 ;
            while(cmd_list[i] != NULL){
                if(strncmp(cmd_list[i],"else",4)==0) { 
                    // else found 
                    else_locn = i ; 
                }else if(strncmp(cmd_list[i],"fi")==0){
                    // End encountered
                    fi_locn = i;
                    break;
                }
                i++;
            }
            ret_val = execute(&cmd_list,else_locn);
        } 
    }










}
