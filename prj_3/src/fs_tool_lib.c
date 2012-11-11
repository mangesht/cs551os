#include <sys/cdefs.h>
#include <string.h>
#include <stdio.h>
#include <lib.h>
#include <unistd.h>

PUBLIC int fileinfo(char *fname,int mode = 3) {
  message m;
  m.m1_i1 = strlen(fname) + 1 ; 
  m.m1_p1 = fname ;
  m.m1_i2 = mode;
  printf("In Lib : Getting info for file %s \n",m.m1_p1); 
  return(_syscall(VFS_PROC_NR,FILEINFO,&m));
}
