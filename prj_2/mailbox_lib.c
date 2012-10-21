

// message 7 data structure for reference 
// It usage in our project

// {int m7i1, m7i2, m7i3, m7i4, m7i5; char *m7p1, *m7p2;} mess_7;
// m7i1    -> Calling process User Id 
// m7i2    -> Calling process Group Id
// m7p1    -> String message 
// m7p2    -> List of destinations for this message 

/* Since getuid and getgid calls are expensive, they would be used only during creating new mailboxes and registration.
*/

PUBLIC int deposit(int *dst, char *msg){
    message mes;
 //   mes.m7i1 = getuid();
 //   mes.m7i2 = getgid();
    mes.m_source = getpid();
//    mes.m_type = 7 ;
    mes.m7p1 = msg; 
    mes.m7p2 = dst;
    return ( (int) _syscall(PM_PROC_NR,DEPOSIT,&mes));
  // For referece  return( (uid_t) _syscall(PM_PROC_NR, GETUID, &m));
}
PUBLIC int retrieve(int source , char *message) 
{
    message mes;
//    mes.m7i1 = getuid();
//    mes.m7i2 = getgid();
    mes.m_source = getpid();
//    mes.m_type = 7 ;
    mes.m7p1  = msg;
    return ( (int) _syscall(PM_PROC_NR,RETRIEVE,&mes));
}
PUBLIC int create_mailbox(int permissions)
{
    message mes;
    mes.m7i1 = getuid();
    mes.m7i2 = getgid();
    mes.m_source = getpid();
//    mes.m_type = 7 ;
    mes.m7i3 = permissions;
    return ( (int) _syscall(PM_PROC_NR,CREATE_MB,&mes));
}

PUBLIC int destroy_mailbox(int id) 
{
    message mes;
//    mes.m7i1 = getuid();
//    mes.m7i2 = getgid();
    mes.m_source = getpid();
//    mes.m_type = 7 ;
    mes.m7i3 = id;
    return ( (int) _syscall(PM_PROC_NR,DESTROY_MB,&mes));

}
PUBLIC int get_av_mailboxes(int *mb_list)
{
    message mes;
    mes.m7i1 = getuid();
    mes.m7i2 = getgid();
    mes.m_source = getpid();
//    mes.m_type = 7 ;
    mes.m7p1 = mb_list;
    return ( (int) _syscall(PM_PROC_NR,GET_AV_MB,&mes));
}

PUBLIC int register(int mb_id) {

    message mes;
    mes.m7i1 = getuid();
    mes.m7i2 = getgid();
    mes.m_source = getpid();
//    mes.m_type = 7 ;
    mes.m7i3 = mb_id;
    return ( (int) _syscall(PM_PROC_NR,REGISTER,&mes));

}
PUBLIC int get_senders(int *sender_list)
{
    message mes;
//    mes.m7i1 = getuid();
//    mes.m7i2 = getgid();
    mes.m_source = getpid();
//    mes.m_type = 7 ;
    mes.m7p1 = sender_list;
    return ( (int) _syscall(PM_PROC_NR,GET_SENDERS,&mes));
}
