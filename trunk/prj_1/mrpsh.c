//----------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementatiom of Operating Systems
// Fall 2012 , Group 3 
// 
//-Description   -------------------------------------------
// This is the main controller for mrpsh shell. 
//----------------------------------------------------------


// Global signal declaration 
static int dbg;
static int debug_en;
static char *PROMPT="MRP:>";
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
#include<fcntl.h>
#include "exe.c"
#include "signal.c"

char ** get_non_empty_line(int fd);
void show_prompt();
char * read_command();

int main(int argc,char *argv[]) {
    int i,j,k;
    char **line_list;
    char **cmd_list;
    int ret_val=-1;
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
    int prf_fd;
    int pid;
    int status;
    profile_file = (char *) malloc(256);
    username = getenv("USER");
    if(debug_en == 1 ) printf("UserName = %s \n",username);
    if(strcmp(username,"root")==0) { 
        // user is root
        strcpy(profile_file,"/root/PROFILE");
    }else{
        strcpy(profile_file,"/home/");
        strcat(profile_file,username);
        strcat(profile_file,"/PROFILE");
    }
    if(debug_en) printf("Profile Path = %s \n",profile_file);
    
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
                prf_fd = open(profile_file,O_CREAT,S_IRUSR |S_IWUSR);
            }
        }
    } 
    if(prf_fd == -1) {
        perror("error");
        printf("Could not read PROFILE. Continuing with default options\n");
    }

    // The program operates in 2 modes 
    // 0 - profile file reading mode 
    // 1 - command promt mode    
    short int cmd_mode = 1 ; 
    line_list = get_non_empty_line(prf_fd);
    int line_num = 0 ;
    int send_bg = 0;
    while(TRUE){
        char* line;
        send_bg = 0 ; 
        if(cmd_mode){
            show_prompt();
            line = read_command();
            if(is_empty(line)){
                free(line);
                continue;
            }
        }else{
            if(line_list[line_num] == NULL) {
                // EOF Reached 
                if(debug_en) printf("Eof reached in PROFILE");
                cmd_mode = 1 ; 
                free(line_list);
                continue;
            }else{
                line = line_list[line_num];
                line_num++;
            }
        }
        if(debug_en) printf("Parse : %s\n",line);

        // parser should give back cmd_list
        // this is temporary arragement for testing 
        cmd_list = (char **) malloc(18*sizeof(char *));
        cmd_list[0] = line; 
       /* //cmd_list[0] = "if"; 
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
        int len=0; 
        while(cmd_list[len]!=NULL) len++;
        printf("Len of command %d \n",len);
        if(strcmp(cmd_list[len-1],"&")==0) 
            send_bg = 1 ;

        pid = fork();
        if(pid == -1 ) {
            perror("fork_error:");
            exit(-1);
        }else if(pid != 0) {
            // Parent process 
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
        free(line);
        //free(line_list);
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

char ** get_non_empty_line(int fd){
    int empty_line = TRUE;
    int bytes_read;
    char *buf;
    int eof = 0;
    char *str;
    int i;
    char **cmd_list;
    int line_num = 0;
    int idx=0;
    cmd_list = (char **) malloc(256);
    if(cmd_list == NULL) {
        printf("Memory allocation failed\n");
        perror("error");
        exit(-1);
    }
    buf = (char *) malloc(256);
    while(!eof){
        //printf("WhileE\n");
        str = (char *) malloc(256);
        if(str == NULL) printf("Memory allocation failed\n");
        bytes_read = read(fd,buf,256);
        if(debug_en) printf("bytes read = %d ",bytes_read); 
        for(i=0;i<bytes_read;i++){
           if(buf[i] == 0xA){
               //Line ends here
               idx = 0;
               //if(empty_line == FALSE) {
                 if(is_empty(str)==FALSE){
                    cmd_list[line_num++] = str;
                    str = (char *) malloc(256);
                    if(str == NULL) printf("Memory allocation failed\n");
               }
               empty_line = TRUE;
           }else{
                str[idx++] = buf[i];
              //  if(buf[i] > 32 && buf[i] < 127 ) {
              //      empty_line = FALSE;
              //  }
           }
        }
        if (bytes_read < 256 ) eof = 1 ; 
  }
  return cmd_list;    
}

char * read_command(){
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
    return cmd;
}

void show_prompt(){
    //printf(":>");
    printf("%s",PROMPT);
}
