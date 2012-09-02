#include<stdio.h>
#include<string.h>

int main(){
    char *str;
    char *str1;
    char ch; 
    int i,i1;
    int j;
    int len;
    str = (char *) malloc(256);
    str1 = (char *) malloc(256);
    printf("Enter the string \n");
    str1 = "Bangalore";
    //i=scanf("%s",str);
    //i1=scanf("%s",str1);
    //scanf("%d"
    j=1;
    while(ch!=10){
        scanf("%c",&ch); 
        //str = strcat(str,&ch);
        len = strlen(str);
        //printf("Strlen = %d \n",len);
        *(str+len) = ch;
        //printf("%x ",ch);
    }
    printf("\n The string is \n %s %s\n",str,str1);
    //printf("\nint1 %d int2 %d \n",i,i1);
    return 0;
}
