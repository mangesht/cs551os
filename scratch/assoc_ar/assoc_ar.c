#include<stdio.h>
#include<stdlib.h>

struct assoc_ar{
    int cur_size;
    char **key_val;
};

void init(struct assoc_ar* ar){
   ar->cur_size = 256;
   ar->key_val = (char **)calloc(256,256);

}

void print_array(struct assoc_ar* ar){
  int i,j=0;
  for(i=0;i<8;i++){
    printf("value = %s\n",ar->key_val[i]);
  }
 
}
char * read_command(){
    char ch; 
    int len = 0;
    int ret_val;
    char * cmd = (char *) malloc(256);
    struct assoc_ar ar;
    init(&ar);
    print_array(&ar);
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
unsigned char get_hash(char *p){
 int i;
 int sum = 0 ;
 for(i=0;p[i]!= NULL;i++){
    sum = sum + (int) *(p+i);
 }
 return (unsigned char)(sum = ((sum & 0xFF) + ((sum >> 8 ) & 0xFF)) & 0xFF);
}


int main(){
  char *p = "Home";
  p = (char *) malloc(256);
  while(1) {
    p = read_command();  
    printf("hash for %s = %d\n",p,(int)get_hash(p));
  }
return 0;

}
