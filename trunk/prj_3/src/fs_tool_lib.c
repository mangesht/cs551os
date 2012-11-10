#include <sys/cdefs.h>
#include <string.h>
#include <stdio.h>
#include <lib.h>
#include <unistd.h>

PUBLIC int fileinfo(char *fname) {
  message m;
  m.m1_p1 = fname ;
  printf("In Lib : Getting info for file %s \n",m.m1_p1); 
  return(_syscall(VFS_PROC_NR,FILEINFO,&m));
}
