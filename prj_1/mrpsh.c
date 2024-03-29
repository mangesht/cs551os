//----------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementation of Operating Systems
// Fall 2012 , Group 3 
// 
//-Description   -------------------------------------------
// This is the main controller for mrpsh shell. 
//----------------------------------------------------------


// Global signal declaration 
static int debug_en;
static char *PROMPT="MRP:>";
static char *home ;

#define TRUE 1
#define FALSE 0
#define STD_INPUT 0
#define STD_OUTPUT 1

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<signal.h>
#include<fcntl.h>
#include "assoc_ar.c"

struct assoc_ar alias_s;
int my_pid ;


#include "exe.c"
#include "signal.c"
#include "parser.c"
#include "valid.c"

int get_non_empty_line(int ,char **);
void show_prompt();
int read_command(char **);
int is_empty(char *);

int alias_fd;
char *alias_file;

void bye(){
    char *p;
    int pid;
    pid = getpid();
    if(pid == my_pid) {
        if(debug_en) printf("Exit called \n");
        p = (char *) malloc(1);
        lseek(alias_fd,0,SEEK_SET);
        alias_fd = open(alias_file,O_TRUNC |O_RDWR);
        write_alias(&alias_s,alias_fd);
        close(alias_fd);
    }
}

int main(int argc,char *argv[]) {
    int i;
    char **cmd_list;
    int ret_val=-1;
    int len=0; 
    int exit_reg_once = 0 ; 
    
    // Register interrupt handler 
    register_signal();
    // Get all the command line parameters 
    for(i=1;i<argc;i++){
        if(strcmp(argv[i],"-debug") == 0 ) {
            debug_en = 1 ;
        }
    }
    // Get the user name and decide the path for PROFILE file
    char *username;
    char *profile_file;
    char *profile_path;
    int prf_fd;
    int pid;
    int status;
    my_pid = getpid();
    printf("my Process Id = %d \n",my_pid);
    // Intialize the alias hash 
    if(debug_en) printf("Initializing hash \n");
    init(&alias_s);
    if(debug_en) printf("Done Initializing hash \n");
    
    profile_file = (char *) malloc(256);
    profile_path = (char *) malloc(256);
    alias_file = (char *) malloc(256);
    username = getenv("USER");
    if(debug_en == 1 ) printf("UserName = %s \n",username);
    if(strcmp(username,"root")==0) { 
        // user is root
        strcpy(profile_file,"/root/PROFILE");
        strcpy(profile_path,"/root/");
    }else{
        strcpy(profile_file,"/home/");
        strcat(profile_file,username);
        strcpy(profile_path,profile_file);
        strcat(profile_path,"/");
        strcat(profile_file,"/PROFILE");
    }
    if(debug_en) printf("Profile Path = %s \n",profile_file);
    home = strdup(profile_path); 
    prf_fd = open(profile_file,O_RDONLY);
    if(prf_fd == -1 ) {
        // Error in file opening
        //printf("Error No = %d",errno); 
        //perror("error");
        if(errno == ENOENT){
            char ch;
            printf("The PRFILE file does not exist at %s ",profile_file);
            printf("Do you want shell to creat default (y/n) ");
            scanf("%c",&ch);
            if(ch != 'n' && ch != 'N') {
                prf_fd =open(profile_file,O_CREAT,S_IRUSR |S_IWUSR); 
            }
        }
    } 
    if(prf_fd == -1) {
        perror("error");
        printf("Could not read PROFILE. Continuing with default options\n");
    }
    
    strcpy(alias_file,profile_path);
    strcat(alias_file,"alias");
    alias_fd = open(alias_file,O_CREAT |O_RDWR,S_IRUSR |S_IWUSR);
    if(alias_fd == -1){
        perror("error:");
        printf("Aliases will not be persistant \n");
    }
    // The program operates in 2 modes 
    // 0 - profile file reading mode 
    // 1 - command promt mode    
    short int cmd_mode = 0 ; 
    int line_full;
    char *line;
    line = (char *) malloc(256*sizeof(char));
    //  line_full=  get_non_empty_line(prf_fd,&line);
    line_full = 1;
    int line_num = 0 ;
    int send_bg = 0;
    cmd_list = (char **) malloc(18*sizeof(char *));
    while(TRUE){
        char* line;
        send_bg = 0 ; 
        if(cmd_mode == 2 ){
            show_prompt();
            read_command(&line);
            if(is_empty(line)){
                continue;
            }
        }else{
            if(line == NULL || line_full == 0) {
                // EOF Reached 
                if(debug_en) printf("Eof reached in PROFILE");
                cmd_mode++ ; 
                if(cmd_mode == 1) {
                    close(prf_fd);
                    if(debug_en) printf("Reading alias\n");
                    line_full = get_non_empty_line(alias_fd,&line);
                    if(debug_en) printf("Reading alias line_0 = %s\n",line);
                    line_num = 0 ;
                }else{
                    if(debug_en) write_alias(&alias_s,STD_OUTPUT);
                    close(alias_fd);
                    chdir(home);
                }
                continue;
            }else{
                if(cmd_mode == 0 ) { 
                    line_full = get_non_empty_line(prf_fd,&line);
                } else {
                    line_full = get_non_empty_line(alias_fd,&line);
                }
                
                line_num++;
            }
            if(line_full == 0) continue ;
        }
        if(debug_en) printf("Parse : %s\n",line);
        
        // parser should give back cmd_list
        for(i=0;i<18;i++){
            cmd_list[i] = NULL;
        }
        parseCmd(line,&cmd_list);
        // this is temporary arragement for testing 
        //    cmd_list[0] = line; 
        /*   cmd_list[0] = "if"; 
        cmd_list[1] = line; 
        cmd_list[2] = "then"; 
        cmd_list[3] = "/bin/ls"; 
        cmd_list[4] = "else"; 
        cmd_list[5] = "/bin/ls -al"; 
        cmd_list[6] = "fi"; 
        cmd_list[7] = "/bin/ls"; 
        // cmd_list[8] = "&"; 
        //cmd_list[2] = "outfile.txt";
        */  
        int no_fork = 0;
        if(debug_en) printf("Parser output \n");
        len = 0;
        while(cmd_list[len]!=NULL){
            if(debug_en) printf("cmd[%d] ->%s<-\n",len,cmd_list[len]);
            len++;
        }
        
        if(validateCommand(&cmd_list,len) != VALID_COMMAND ) { 
            printf("Invalid command \n");
            continue;
        }
        if(debug_en) printf("Validation output \n");
        len = 0;
        while(cmd_list[len]!=NULL){
            if(debug_en) printf("cmd[%d] =%s \n",len,cmd_list[len]);
            len++;
        }
        
        if(debug_en) printf("Len of command %d \n",len);
        if(strcmp(cmd_list[len-1],"&")==0) 
        send_bg = 1 ;
        if(
        (strncmp(cmd_list[0],"cd ",3)==0) ||
        (strcmp(cmd_list[0],"cd")==0) ||
        (strcmp(cmd_list[0],"set")==0) ||
        (strcmp(cmd_list[0],"alias")==0) ||
        (strcmp(cmd_list[0],"exit"))==0){
            if(debug_en) printf("cd matched");
            no_fork = 1 ; 
            
            if(strcmp(cmd_list[0],"alias")==0) {
                ret_val = set_alias(cmd_list[1],cmd_list[2]);
                continue;
            }else if(strcmp(cmd_list[0],"set")==0) {
                ret_val = set_config(cmd_list[1],cmd_list[2]);
                continue;
            }
            
        }
        if(no_fork == 1 ) {
            ret_val = execute(&cmd_list,0);
        } else {
            pid = fork();
            if(pid == -1 ) {
                perror("fork_error:");
                exit(EXIT_FAILURE);
            }else if(pid != 0) {
                // Parent process 
                if(exit_reg_once == 0 ) { 
                    i	 = atexit(bye);
                    exit_reg_once = 1 ;
                }
                if(send_bg == 0 ) { 
                    waitpid(-1,&status,0);
                    if(WIFEXITED(status)) {
                        if(debug_en) printf("Child %d exited properly\n",pid);
                    }else{
                        if(debug_en) printf("Child %d NOT exited properly\n",pid);
                    }
                }else{
                    printf("bg %d\n",pid);
                }
            }else{
                if(debug_en) printf("child : %s cmd_address = %x \n",cmd_list[0],(int)&cmd_list);
                ret_val = execute(&cmd_list,0);
                if(ret_val == -1 ) {
                    perror("exe_error:");
                    return 0;
                }else{
                    return 1;
                }
            }
        }
    } 
    return 0;
}

int is_empty(char *line){
    int len;
    int i;
    int res=TRUE;
    len  = strlen(line);
    for(i=0;i<len;i++){
        if(line[i] > 32 && line[i] < 127 ) {
            res = FALSE ; 
            break;
        }
    }
    return res;
}

int get_non_empty_line(int fd,char **line) {
    int idx;
    int bytes_read;
    char *buf;
    char *str; 
    str = *line;
    isMemAllocated(buf = (char *) malloc(4));
    idx = 0;
    while((bytes_read = read(fd,buf,1))>0){
        if (debug_en) printf("Read from file  %c  %d \n",buf[0],buf[0]);
        if(buf[0] == 0xA) {
            *(str+idx) = 0;
            break;
        }else{
            *(str+idx) = buf[0];
            idx++;
            *(str+idx) =0;
        }
    }
    if (debug_en) printf("output = %s \n",*line);
    //*line = str;
    return idx;
}

int  read_command(char **in_cmd){
    char ch; 
    int len = 0;
    int ret_val;
    char * cmd = (char *) malloc(256);
    ch = 0 ; 
    while(ch!=10){
        ret_val = scanf("%c",&ch); 
        if(ret_val == -1) {
            perror("scan error");
            break;
        }
        *(cmd+len) = ch;
        len++;
    }
    *(cmd+len-1) = 0;
    //printf("scanDone %s",cmd);
    //printf("cmd = %x str = %x \n",cmd,str);
    *in_cmd = cmd;
    return 1;
}

void show_prompt(){
    //printf(":>");
    printf("%s",PROMPT);
}


