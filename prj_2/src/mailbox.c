#include "fs.h"
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include<stdio.h>
#include<errno.h>
#include<stdarg.h>
#include <string.h>
#include <minix/callnr.h>
#include <minix/safecopies.h>
#include <minix/endpoint.h>
#include <minix/com.h>
#include <minix/sysinfo.h>
#include <minix/u64.h>
#include <sys/ptrace.h>
#include <sys/svrctl.h>
#include "file.h"
#include "fproc.h"
#include "scratchpad.h"
#include "dmap.h"
#include <minix/vfsif.h>
#include "vnode.h"
#include "vmnt.h"
#include "param.h"


FILE *logFile = NULL;

void lprint(char * format, ...)
{
    va_list args;
    if(logFile==NULL){
        logFile = fopen("/usr/log.ini","w");
         if(logFile == NULL ) {
            printf("ERROR: Could not open file\n");
         }
        
    }
    va_start(args,format);
    vfprintf(logFile,format,args);
    fflush(logFile);
    va_end(args);

}


PUBLIC int do_deposit()
{
   printf("Do deposit called \n");
   lprint("Do deposit called lp\n");
   return 0;
}

PUBLIC int do_retrieve() 
{
    return 0;
}
PUBLIC int do_destroy_mailbox() 
{
    return 0;
}
PUBLIC int do_create_mailbox()
{
    return 0;
}
PUBLIC int do_get_av_mailboxes()
{
    return 0;
}
PUBLIC int do_register_mb() 
{
    return 0;
}

PUBLIC int do_get_senders()
{
    return 0;
}

