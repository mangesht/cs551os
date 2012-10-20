#include<stdio.h>

int main(){
int dest[2] = { 1234,2345};
char  msg[256];
int res;
res = deposit(&dest,&msg);
printf("Returned %d ",res);

return 0;
}
