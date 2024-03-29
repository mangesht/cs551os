//----------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementatiom of Operating Systems
// Fall 2012 , Group 3 
// 
//-Description   -------------------------------------------
// The function execute and execute single perform the system calls required to execute the user asked functions.
//----------------------------------------------------------

#define PIPE_READ 0 
#define PIPE_WRITE 1 

int set_alias(char * key,char *val){
    //add functionality 
    int r;
    if(debug_en) printf("adding alias %s %s \n",key,val);
    r = add(&alias_s,key,val);
    return r;
}

int set_config(char * key,char *val){
    //add functionality 
    if(strcmp(key,"home")==0) {
        //strcpy(home ,val );
        home = strdup(val);
    }else if(strcmp(key,"prompt")==0){
        //strcpy(PROMPT,val);
        PROMPT = strdup(val);
    }
    //printf("\n");
    return 0;
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
            if(i==len-1){
                tstr[idx++] = '\0';
                str_list[line_num]= tstr;
                line_num++;
            }
        }else if((skip == 0 && str[i] == 0x20)){
            tstr[idx++] = '\0';
            str_list[line_num]= tstr;
            line_num++;
            idx = 0 ;
            //printf(" m2 "); 
            tstr = (char *) malloc(256);
            if(tstr == NULL) printf("Alloc error");
        }else if(i==len-1){
            *(tstr+idx) = str[i];
            idx++;
            tstr[idx++] = '\0';
            str_list[line_num]= tstr;
            line_num++;
        }else{
            *(tstr+idx) = str[i];
            idx++;
            //printf(" m3 "); 
            //printf("%d ",i);
        }
    }
    str_list[line_num] = NULL;
}
int execute_single(char * cmd){
    char **cmd_list;
    int ret_val=0;
    if(debug_en) printf("executing single ->%s<-\n",cmd); 
    cmd_list = (char **) malloc((count_spaces(cmd)+3)*sizeof(char *));
    split(cmd,&cmd_list);
    if(debug_en) printf("\nInput to execv\n");
    for(ret_val=0;cmd_list[ret_val] != NULL;ret_val++) {
        if(debug_en)printf("cmd_list[%d] = %s\n",ret_val,cmd_list[ret_val]);
    }
    
    if(strcmp(cmd_list[0],"cd")==0) {
        if(debug_en) printf("Chaning directory\n");
        if(cmd_list[1] == NULL){ 
            cmd_list[1] = (char * ) malloc(256);
            //cmd_list[1] = strdup(home);
            strcpy(cmd_list[1],home);
        }
        //printf("dir to %s of len %d Home = %s\n",cmd_list[1],strlen(cmd_list[1]),home);
        ret_val = chdir(cmd_list[1]);
    }else if(strcmp(cmd_list[0],"set")==0) {
        // shell specific configurations
        ret_val =  set_config(cmd_list[1],cmd_list[3]);
    }else if(strcmp(cmd_list[0],"exit")==0) {
        exit(EXIT_SUCCESS);
    }else{
        ret_val = execv(cmd_list[0],cmd_list);
    }
    if(ret_val == -1){
        perror("execution error:");
        ret_val = 0 ;
    }
    return ret_val;
}
int execute(char ***cmd_list_ptr,int start_idx){
    char **cmd_list;
    int ret_val;
    int i;
    int len;
    int pipe_fd[2];
    cmd_list = *cmd_list_ptr;
    len = 0 ;
    if(debug_en) printf("executing %s\n",cmd_list[start_idx]); 
    while(cmd_list[start_idx+len] != NULL) { 
        len++;
    }
    //printf("Len = %d \n",len); 
    if(len > 2 ) {
        if(strcmp(cmd_list[start_idx+1],">")==0 || strcmp(cmd_list[start_idx+1],"|")==0) { 
            if(debug_en) printf("processing operator %s \n",cmd_list[start_idx+1]);
            if(strcmp(cmd_list[start_idx+1],">")==0){
                int out_fd;
                if(debug_en) printf("processing redirection operator %s \n",cmd_list[start_idx+1]);
                // Redirection operator
                //out_fd = open(cmd_list[start_idx+2],O_CREAT,0x777);
                //out_fd = open("outfile.txt",O_CREAT|O_WRONLY,S_IRUSR |S_IWUSR);
                out_fd = open(cmd_list[start_idx+2],O_CREAT | O_TRUNC | O_RDWR,S_IRUSR |S_IWUSR);
                if(out_fd == -1 ) {
                    if(debug_en) printf("Error Opening file :%s:",cmd_list[start_idx+2]);
                    
                    // Error in file opening
                    perror("error_redirect:");
                    return -1;
                }
                close(STD_OUTPUT);
                dup(out_fd);
                close(out_fd);
                ret_val=execute_single(cmd_list[start_idx]);
            }else if(strcmp(cmd_list[start_idx+1],"|")==0){
                // pipe operator
                int pid;
                if((pipe(&pipe_fd[0])) == -1 ) {
                    perror("pipe_error:");
                    return 0;
                }
                pid = fork();
                if(debug_en) printf("pid = %d \n",pid);
                //printf("Pipeing \n");
                if(pid == -1 ) {
                    perror("forking_error:");
                    return 0;
                }else if(pid == 0) {
                    // Child process 
                    close(pipe_fd[PIPE_READ]);
                    close(STD_OUTPUT);
                    dup(pipe_fd[PIPE_WRITE]);
                    close(pipe_fd[PIPE_WRITE]);
                    ret_val = execute_single(cmd_list[start_idx]);
                    //execute(&cmd_list,start_idx );
                }else{
                    // Parent process
                    close(pipe_fd[PIPE_WRITE]);
                    close(STD_INPUT);
                    dup(pipe_fd[PIPE_READ]);
                    close(pipe_fd[PIPE_READ]);
                    //ret_val = execute_single(cmd_list[start_idx+2]);
                    execute(&cmd_list,start_idx + 2 );
                }
            }
            //ret_val = execute_single(cmd_list[start_idx+2]);
        }else if(strcmp(cmd_list[start_idx],"if")==0) {
            // Process it as if then else fi
            if(debug_en) printf("Processing if\n");
            // To get return value from this process , we will fork this one , if child process runs successfully it wont return anything else it returns erorr value . We pass this value using a pipe
            int mb[2];
            int pid;
            int pipe_get_num;
            if(pipe(&mb[0]) == -1) {
                perror("pipeing_error:");
                return 0;
            }
            int else_locn = -1;
            int then_locn = -1;
            int fi_locn = -1;
            int if_cnt = 0 ; 
            i = start_idx + 1 ;
            while(cmd_list[i] != NULL){
                if(strcmp(cmd_list[i],"else")==0 && if_cnt == 0 ) { 
                    // else found 
                    else_locn = i ; 
                }else if(strcmp(cmd_list[i],"fi")==0){
                    // End encountered
                    if(if_cnt == 0) { 
                        fi_locn = i;
                        break;
                    } else { 
                        if_cnt--;
                    }
                } else if(strcmp(cmd_list[i],"then")==0 && if_cnt == 0 ) { 
                    then_locn = i;
                }else if(strcmp(cmd_list[i],"if")==0){
                    if_cnt++;
                }
                i++;
            }
            
            pipe_get_num = 0 ; 
            pid = fork();
            if(debug_en) printf("pid = %d \n",pid);
            if(pid == -1 ) {
                perror("forking_error:");
                return 0;
            }else if(pid != 0) {
                // Parent process 
                char buf;
                int status;
                close(mb[PIPE_WRITE]);
                waitpid(pid,&status,0);
                pipe_get_num = read(mb[PIPE_READ],&buf,1); 
                close(mb[PIPE_READ]);
                if(pipe_get_num > 0){
                    ret_val = 0 ;
                }else{
                    ret_val = 1 ; 
                }
            }else{
                char str[] = "Fail";
                close(mb[PIPE_READ]);
                //ret_val = execv(cmd_list[0],cmd_list,0);
                ret_val = execute(&cmd_list,start_idx+1);
                write(mb[PIPE_WRITE],str,3);
                close(mb[PIPE_WRITE]);
                if(ret_val == -1 ) {
                    perror("run error:");
                    return -1;
                }else{
                    return 1;
                }
            }
            //ret_val = execute(&cmd_list,start_idx+1);
            if(debug_en) printf("if ret_val= %d , else_locn = %d fi_locn = %d ",ret_val,else_locn,fi_locn);
            if(ret_val == TRUE ){
                // Find then part , it must be at cmd_list[start_idx+2]
                ret_val = execute(&cmd_list,then_locn+1);
            }else{
                // execute the false part of if ()
                ret_val = execute(&cmd_list,else_locn+1);
            } 
        }else if(strcmp(cmd_list[start_idx],"cd")==0) {
            // Change directory
        }else{
            if(debug_en) printf("No match elsewhere\n");
            ret_val = execute_single(cmd_list[start_idx]);
        }
    }else{
        if(debug_en) printf("Len smaller \n");
        ret_val = execute_single(cmd_list[start_idx]);
    }
    return ret_val;
}



