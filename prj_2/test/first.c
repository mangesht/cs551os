#include<stdio.h>
#include <string.h>

int main(){
int dest[2] = { 1234,2345};
char  msg[256];
int res;
//msg = "Hello";
strcpy(msg,"Hello");
//res = deposit(&dest,&msg);
res = create_mailbox(0x4);
printf("Returned %d ",res);

return 0;
}
