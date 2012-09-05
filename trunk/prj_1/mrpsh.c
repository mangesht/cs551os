//----------------------------------------------------------
// Illinois Institute of Technology, Chicago
// CS 551 Implementatiom of Operating Systems
// Fall 2012 , Group 3 
// 
//-Description   -------------------------------------------
// This is the mail controler for mrpsh shell. 
//----------------------------------------------------------


// Global signal declaration 
static int dbg;
#define TRUE 1
#define FALSE 0

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>


char ** get_non_empty_line(int fd);
void show_prompt();
char * read_command();

static int debug_en;
int main(int argc,char *argv[]) {
    int i,j,k;
    char **line_list;
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
                prf_fd = open(profile_file,O_CREAT);
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
    short int cmd_mode = 0 ; 
    line_list = get_non_empty_line(prf_fd);
    int line_num = 0 ;
    while(TRUE){
        char ch;
        char* line;
        if(cmd_mode){
            show_prompt();
            line = read_command();
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
        free(line);
        //free(line_list);
    } 
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
               if(empty_line == FALSE) {
                    cmd_list[line_num++] = str;
                    str = (char *) malloc(256);
                    if(str == NULL) printf("Memory allocation failed\n");
               }
               empty_line = TRUE;
           }else{
                str[idx++] = buf[i];
                if(buf[i] > 32 && buf[i] < 127 ) {
                    empty_line = FALSE;
                }
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
    *(cmd+len-1) = NULL;
    //printf("scanDone %s",cmd);
    //printf("cmd = %x str = %x \n",cmd,str);
    return cmd;
}

void show_prompt(){
    printf(":>");
}