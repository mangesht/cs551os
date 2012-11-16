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


int debug_en = 0 ; 
int get_non_empty_line(int ,char **);
void display_help(){
   printf("\n Usage : fraginfo [filename] [-i/-e] [-b] [-h] \n");
   printf("     -i  : Selects Internal fragmentation for reporting\n");   
   printf("     -e  : Selects External fragmentation for reporting\n");   
   printf("     -d  : Gets fragmentation info for complete FS\n");   
   printf("     -h  : gets the help how to use this tool \n"); 
}
int main(int argc,char *argv[]) { 
   int retVal;
   int mode_i = 0  ;
   int agcCount =1 ;
   int got_f_name;
   int got_opt;
   int frag_selected = 0;
   int int_ext = 0; // 0 for internal,  1 for external
   char *p;
   char *fname;
   p = (char *) malloc (256);
   fname =  (char *) malloc (256);
   //strcpy(p , argv[agcCount]);
   //printf("Atgc = %d \n",argc);
   got_f_name = 0 ;
   got_opt = 0 ;
   // This is default filename 
   strcpy(fname, "/home/mthakare/test/sample.txt");
   while(agcCount < argc){ 
     strcpy(p , argv[agcCount]);
     //printf("p = %s C= %c",p,p[0] );
     //printf("agcCount = %d ",agcCount);
     //printf("\n");
     if(p[0] == '-'){ 
        //printf("option selected \n");
        got_opt = 1 ;
        if(strchr(p,'d') != NULL) {
            printf("device selected \n");
            mode_i = mode_i | 0x10 ;
        }
        if(strchr(p,'h') != NULL) {
            //printf("Help selected \n");
            display_help();
            return 0;
        }
        if(strchr(p,'i') != NULL) {
            //printf("Internal selected \n");
            if(frag_selected == 1 ) { 
                display_help();
                return 0;
            }
            frag_selected = 1 ;
            int_ext = 0;
        }
        if(strchr(p,'e') != NULL) {
            //printf("External selected \n");
            if(frag_selected == 1 ) { 
                display_help();
                return 0;
            }
            frag_selected = 1 ;
            int_ext = 1;
        }


        
     }else{
         // This must be filename
         if(got_f_name == 1 ) {
             display_help();
             return 0;
         }
         strcpy(fname,p);
         got_f_name = 1 ;
     }
     agcCount++;   
   } 
   if(frag_selected == 0 || ( int_ext == 0 )) {
   if(got_f_name == 1) { 
       mode_i = 0x4 ;
       printf("Mode = %x \n",mode_i);
       printf("FileName in test = %s \n",fname);
       retVal = fileinfo(fname,mode_i);
   
      if(retVal == -1) { 
          perror("fileinfo:");
       }
   }else{
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
                   //printf("NO Matched for %s +++++  %s \t",cmd_list[0],cmd_list[1]);
                   //printf("Hence running it \n"); 
                   strcpy(last_dev,cmd_list[0]);
                   mode_i = 0x4;
                   retVal = fileinfo(cmd_list[1],mode_i);
               }
       
            }
         }
    }       
    } else { 
        // External fragmentation
       retVal = fraginfo(fname,mode_i);
       if(retVal == -1) { 
          perror("fraginfo:");
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
    buf = (char *) malloc(4);
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
