#include <lib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <minix/callnr.h>
#include <minix/safecopies.h>

// List of functions defined here 
PUBLIC int deposit(int *dst, char *msg);
PUBLIC int retrieve(int source , char *msg);
PUBLIC int destroy_mailbox(int id);
PUBLIC int create_mailbox(int permissions);
PUBLIC int get_av_mailboxes(int *mb_list);
PUBLIC int register_mb(int mb_id) ;
PUBLIC int get_senders(int *sender_list);

//

// message 7 data structure for reference 
// It usage in our project

// {int m7i1, m7i2, m7i3, m7i4, m7i5; char *m7p1, *m7p2;} mess_7;
// m7i1    -> Calling process Id PID 
// m7i2    -> Calling process User Id 
// m7i3    -> Calling process Group Id
// m7p1    -> String message 
// m7p2    -> List of destinations for this message 

/* Since getuid and getgid calls are expensive, they would be used only during creating new mailboxes and registration.
*/

PUBLIC int deposit(int *dst, char *msg){
    message mes;
 //   mes.m7i2 = getuid();
 //   mes.m7i3 = getgid();
    mes.m7_i1 = getpid();
//    mes.m_type = 7 ;
    mes.m7_p1 = msg; 
    mes.m7_p2 = dst;
    printf("Process %d sending message = %s to ",mes.m7_i1,msg);
    return ( (int) _syscall(VFS_PROC_NR,DEPOSIT,&mes));
  // For referece  return( (uid_t) _syscall(VFS_PROC_NR, GETUID, &m));
}
PUBLIC int retrieve(int source , char *msg) 
{
    message mes;
//    mes.m7i2 = getuid();
//    mes.m7i3 = getgid();
    mes.m7_i1 = getpid();
//    mes.m_type = 7 ;
    mes.m7_p1  = msg;
    return ( (int) _syscall(VFS_PROC_NR,RETRIEVE,&mes));
}
PUBLIC int create_mailbox(int permissions)
{
    message mes;
    mes.m7_i2 = getuid();
    mes.m7_i3 = getgid();
    mes.m7_i1 = getpid();
//    mes.m_type = 7 ;
    mes.m7_i4 = permissions;
    return ( (int) _syscall(VFS_PROC_NR,CREATE_MB,&mes));
}

PUBLIC int destroy_mailbox(int id) 
{
    message mes;
//    mes.m7_i2 = getuid();
//    mes.m7_i3 = getgid();
    mes.m7_i1 = getpid();
//    mes.m_type = 7 ;
    mes.m7_i3 = id;
    return ( (int) _syscall(VFS_PROC_NR,DESTROY_MB,&mes));

}
PUBLIC int get_av_mailboxes(int *mb_list)
{
    message mes;
    mes.m7_i2 = getuid();
    mes.m7_i3 = getgid();
    mes.m7_i1 = getpid();
//    mes.m_type = 7 ;
    mes.m7_p1 = mb_list;
    return ( (int) _syscall(VFS_PROC_NR,GET_AV_MB,&mes));
}

PUBLIC int register_mb(int mb_id)
{
    message mes;
    mes.m7_i2 = getuid();
    mes.m7_i3 = getgid();
    mes.m7_i1 = getpid();
//    mes.m_type = 7 ;
    mes.m7_i4 = mb_id;
    return ( (int) _syscall(VFS_PROC_NR,REGISTER,&mes));
}
PUBLIC int get_senders(int *sender_list)
{
    message mes;
//    mes.m7_i2 = getuid();
//    mes.m7_i3 = getgid();
    mes.m7_i1 = getpid();
//    mes.m_type = 7 ;
    mes.m7_p1 = sender_list;
    return ( (int) _syscall(VFS_PROC_NR,GET_SENDERS,&mes));
}
