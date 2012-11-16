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
#include "../../prj_1/parser.c"
int debug_en = 0 ; 

int get_non_empty_line(int ,char **);
//int parseCmd(char * in_cmd, char *** cmd_list);

int main() {
  char **cmd_list;
  char *last_dev;
  int line_full;
  int alias_fd;
   int i;
  char *alias_file;
  char *line;
  line = (char *) malloc(256*sizeof(char));
  system("/home/mthakare/cs551os/prj_3/tool/list_dev");
  cmd_list = (char **) malloc(18*sizeof(char *));
  cmd_list[0] = (char *) malloc(256*sizeof(char));
  cmd_list[1] = (char *) malloc(256*sizeof(char));
  alias_file = (char *) malloc(256);
  last_dev = (char *) malloc(256);
  strcpy(last_dev,"");
  strcpy(alias_file ,"/home/mthakare/dev.ini"); 
  alias_fd = open(alias_file,O_RDONLY);
  if(alias_fd == -1) { 
        printf("Error reading wriring devices \n");
        return 0;
  }
  while(1) { 
  line_full = get_non_empty_line(alias_fd,&line);
  if(line_full == 0 ) {
        // EOF Reached 
        break;
        
  }else{
    //    for(i=0;i<18;i++){
    //       cmd_list[i] = NULL;
    //    }
     //   parseCmd(line,&cmd_list);
        int j = 0 ;
        int k=0;
        char *p;
       // printf("processing \n");
        for(i=0,k=0; k<strlen(line);k++)  { 
         // printf( "%d ",line[k]);
          if(line[k] == 0x20){
                cmd_list[i][j] = '\0';
                if(i==0) j = 0;
                i++; 
          }else{
            cmd_list[i][j] = line[k];
            j++;
          }
        }
       // printf("\n");
       cmd_list[i][j] = NULL; 
      // printf("Assigning null to i = %d j = %d \n",i,j);
      //  printf("parsing line = %s \n",line);
      //  printf("Output = %s %s \n",cmd_list[0],cmd_list[1]);

        if(strcmp(last_dev,cmd_list[0]) == 0 ) { 
            //printf("Matched for %s %s \n",cmd_list[0],cmd_list[1]);
        }else{
            if(strcmp(cmd_list[0],"2") == 0 ) { 
                continue;
            }
            printf("NO Matched for %s +++++  %s \t",cmd_list[0],cmd_list[1]);
            printf("Hence running it \n"); 
            strcpy(last_dev,cmd_list[0]);
        }

  }
  }
  return 0;
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


