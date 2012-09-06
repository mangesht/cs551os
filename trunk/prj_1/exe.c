//----------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementatiom of Operating Systems
// Fall 2012 , Group 3 
// 
//-Description   -------------------------------------------
// The function execute and execute single perform the system calls required to execute the user asked functions.
//----------------------------------------------------------

void set_alias(char * key,char *val){
        //add functionality 
        printf("\n");
}

void set_config(char * key,char *val){
        //add functionality 
        printf("\n");
}
int count_spaces(char *str){
    int skip = 0 ; 
    int num_spaces=0;
    int len;
    int i;
    len = strlen(str);
    for(i=0;i<len;i++){
        if(str[i] == 0x22 ) {
            // This indicates quote "
            skip = skip == 1 ? 0 : 1 ; 
        }else if(skip == 0 && str[i] == 0x20){
            num_spaces++;
        }
    }
    return num_spaces;
}


void split(char *str,char *** str_list_ptr){
    char ** str_list;
    char *tstr;
    int i;
    int num_spaces = 0 ;
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
    //str_list = (char **) malloc(num_spaces * sizeof(char *));
    str_list = *str_list_ptr;
    skip = 0 ;
    line_num = 0;
    idx = 0 ;
    tstr = (char *) malloc(256);
    //printf("strlen of %s = %d \n",str,len);
    // Split the input string using space as delimeter
    for(i=0;i<len;i++){
         //printf("\n processing   %c",str[i]);
         if(str[i] == 0x22 ) {
            // This indicates quote "
            //printf(" m1 "); 
            skip = skip == 1 ? 0 : 1 ; 
        }else if((skip == 0 && str[i] == 0x20)){
            //*(tstr+idx) = 0;
            //*(str_list+line_num) = tstr;
            str_list[line_num]= tstr;
            line_num++;
            idx = 0 ;
            //printf(" m2 "); 
            tstr = (char *) malloc(256);
            if(tstr == NULL) printf("Alloc error");
        }else if(i==len-1){
            *(tstr+idx) = str[i];
            str_list[line_num]= tstr;
        }else{
            *(tstr+idx) = str[i];
            idx++;
            //printf(" m3 "); 
            //printf("%d ",i);
        }
    }
}
int execute_single(char * cmd){
    char **cmd_list;
    int ret_val=0;
    if(debug_en) printf("executing single %s\n",cmd); 
    cmd_list = (char **) malloc(count_spaces(cmd)*sizeof(char *));
    split(cmd,&cmd_list);
    //printf("Input to execv\n");
    //for(ret_val=0;cmd_list[ret_val] != NULL;ret_val++) {
        //printf("cmd_list[%d] = %s\n",ret_val,cmd_list[ret_val]);
    //} 
    ret_val = execv(cmd_list[0],cmd_list,0);
    if(ret_val == -1){
        perror("execution error:");
    }
    printf("Give us chance to free mem\n");
    return ret_val;
}
int execute(char ***cmd_list_ptr,int start_idx){
    char **cmd_list;
    int ret_val;
    int i;
    int next_cmd_locn = start_idx;
    cmd_list = *cmd_list_ptr;
    
    if(debug_en) printf("executing %s\n",cmd_list[start_idx]); 
    if(strncmp(cmd_list[start_idx+1],">",1)==0 || strncmp(cmd_list[start_idx+1],"|",1)==0) { 
        if(strncmp(cmd_list[start_idx+1],">",1)==0){
            // Redirection operator
        }else if(strncmp(cmd_list[start_idx+1],"|",1)==0){
            // pipe operator
        }
        ret_val = execute_single(cmd_list[start_idx+2]);
    }else if(strncmp(cmd_list[start_idx],"if",2)==0) {
        // Process it as if then else fi
        ret_val = execute(&cmd_list,start_idx+1);
        // search for else
        int else_locn = -1;
        int fi_locn = -1;
        i = start_idx + 1 ;
        while(cmd_list[i] != NULL){
            if(strncmp(cmd_list[i],"else",4)==0) { 
                // else found 
                else_locn = i ; 
            }else if(strncmp(cmd_list[i],"fi",2)==0){
                // End encountered
                fi_locn = i;
                break;
            }
            i++;
        }

        if(ret_val == TRUE ){
            // Find then part , it must be at cmd_list[start_idx+2]
            ret_val = execute(&cmd_list,start_idx+3);
        }else{
            // execute the false part of if ()
            ret_val = execute(&cmd_list,else_locn);
        } 
        // Remove this if then else from the cmd_list 
        for(i=fi_locn+1;cmd_list[i] != NULL;i++){
            cmd_list[next_cmd_locn++] = cmd_list[i];
        }
        cmd_list[next_cmd_locn++] = NULL;
    }else if(strncmp(cmd_list[start_idx],"alias",5)==0) {
        set_alias(cmd_list[start_idx+1],cmd_list[start_idx+3]);
    }else if(strncmp(cmd_list[start_idx],"set",3)==0) {
        // shell specific configurations
        set_config(cmd_list[start_idx+1],cmd_list[start_idx+3]);
    }else{
        if(debug_en) printf("No match elsewhere\n");
        ret_val = execute_single(cmd_list[start_idx]);
    }
}



