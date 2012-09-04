#include<stdio.h>
#include<string.h>
#include<unistd.h>

int main(){
    char *str;
    char *str1;
    char *buf;
    char ch; 
    int i,i1;
    int j;
    int len;
    int pid;
    str = (char *) malloc(256);
    str1 = (char *) malloc(256);
    printf("Enter the string \n");
    str1 = "Bangalore";
    //i=scanf("%s",str);
    //i1=scanf("%s",str1);
    //scanf("%d"
    j=1;
    if((pid=fork())!= 0) {
      printf("In Parent process \n");
       while(ch!=10){
           scanf("%c",&ch); 
           //str = strcat(str,&ch);
           len = strlen(str);
           //printf("Strlen = %d \n",len);
           *(str+len) = ch;
           //printf("%x ",ch);
           j = 0; 
       }
       kill(pid,9);
    }else{
      printf("In Child process \n");
        // Child process 
        buf = (char *) malloc(10);
        j = 1 ; 
        while(j) { 
            int pos = 10;
            sleep(1);
            buf = "Delhi";
            pos = lseek(1,1,SEEK_SET);
            write(1,buf,5);
            buf = "\r";
            write(1,buf,1);
            //printf("Read %d %s ",pos,buf);
        }
      printf("Done In Child process \n");
    }
    printf("\n The string is \n %s %s\n",str,str1);
    //printf("\nint1 %d int2 %d \n",i,i1);
    return 0;
}
