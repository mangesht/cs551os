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

#define MAX_SENDERS 10
#define MAX_MAILBOXES 10
#define MB_CAPACITY 10

FILE *logFile = NULL;
int mb_id_pool = 0 ; 
int num_active_mailboxes = 0 ;

// Data structures 
struct MailBox{
	int mb_id;
	int owner_pid;  
    int owner_uid;
    int owner_gid;
	int perm;
    int senders[MAX_SENDERS];
    int num_senders;
	struct mnode *front ;
	struct mnode *rear ;
};
MailBoxList mbList[MAX_MAILBOXES];



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
// Helper functions 
int get_free_id() {
   int i;
   for(i=0;i<MAX_MAILBOXES;i++) {
        if(((mb_id_pool >> i) & 0x1 ) == 0 ){
            mb_id_pool = (mb_id_pool | 1<<i)
            printf("mb_id_pool = %x ",mb_id_pool);
            num_active_mailboxes++;
            return i;
        } 
    }
    printf("get_free_id Returning failure \n");
    return -1;
}
int addMailBox(MailBox mb) {
    int mb_id;
	mb_id     = get_free_id();
    if(mb_id < 0) {
        printf("ERROR : mailbox space full \n");
        return -1;
    }else{
        mbList[mb_id] = mb;
    }
    return mb_id;
}
int create_mailbox(int owner_pid,int owner_uid,int owner_gid,int permissions)
{
    int i;
    MailBox mb;
    mb.owner_pid = owner_pid ;
    mb.owner_uid = owner_uid ;
    mb.owner_gid = owner_gid ;
	mb.perm      = permissions;
    for(i=0;i<MAX_SENDERS;i++) { 
        senders[i] = -1 ;
    }
    num_senders = 0 ;
    mb.mb_id = addMailBox(mb);
    return mb.mb_id;
}


// Sys call functions 


PUBLIC int do_deposit()
{
   int my_pid;
   my_pid = getpid();
   printf("Do deposit called Pid = %d my_pid = %d \n",m_in.m7_i1,my_pid);
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
    int rx_pid;
    int rx_uid;
    int rx_gid;
    int mb_id;
    int perm;
    rx_pid = m_in.m7_i1;
    rx_uid = m_in.m7_i2;
    rx_gid = m_in.m7_i3;
    perm   = m_in.m7_i4;
    mb_id = create_mailbox(rx_pid,rx_uid,rx_gid,perm);
    return mb_id;
}
PUBLIC int do_get_av_mailboxes()
{
    int *mailingList;
    int i;
    int tx_pid ;
    mailingList = (int * ) malloc (num_active_mailboxes*2*sizeof(int));
    tx_pid = m_in.m7_i1;
    for(i=0;i<num_active_mailboxes;i++) {
        mailingList[2*i] =mbList[i].mb_id; 
        mailingList[2*i+1] =mbList[i].owner_pid; 
    }
    sys_datcopy(VFS_PROC_NR,mailingList,tx_pid,m_in.m7_p1,2*num_active_mailboxes);
    free(mailingList);
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

