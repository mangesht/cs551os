#include <sys/cdefs.h>
#include <string.h>
#include <stdio.h>
#include <lib.h>
#include <unistd.h>

PUBLIC int fileinfo(char *fname,int mode ) {
  message m;
  m.m1_i1 = strlen(fname) + 1 ; 
  m.m1_p1 = fname ;
  m.m1_i2 = mode;
//  printf("In Lib : Getting info for file %s Mode = %d \n",m.m1_p1,m.m1_i2); 
  return(_syscall(VFS_PROC_NR,FILEINFO,&m));
}


PUBLIC int fraginfo(char *fname,int mode ) {
  message m;
  m.m1_i1 = strlen(fname) + 1 ; 
  m.m1_p1 = fname ;
  m.m1_i2 = mode;
//  printf("In Lib : Getting fragmentation info for file %s Mode = %d \n",m.m1_p1,m.m1_i2); 
  return(_syscall(VFS_PROC_NR,FRAGINFO,&m));
}
