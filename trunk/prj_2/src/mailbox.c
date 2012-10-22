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

int USER = 0x4;
int GROUP = 0x2;
int EVERYONE = 0x1;

FILE *logFile = NULL;
int mb_id_pool = 0 ; 
int num_active_mailboxes = 0 ;
int senderDB[MAX_SENDERS * MAX_MAILBOXES] ; 

/* Mail messages. This will be places in the mailbox */
 struct Message_mb{
   char data[256];
   int sender_id; 
};

// Queue: Provides nodes to hold the mail messages
 struct MailNode{
	struct  Message_mb *msg;
	struct MailNode *next;
};

// structure for suspended senders' process information

struct SuspendContext{
    struct Message_mb *msg;
    int *suspened_for;
    int len_sus_for;
};


// Data structures 
struct MailBox{
	int mb_id;
	int owner_pid;  
    int owner_uid;
    int owner_gid;
	int perm;
    int senders[MAX_SENDERS];
    int suspended_sender[MAX_SENDERS];
    int sus_sender_idx;
    int num_senders;
    int num_messages;
    int rx_suspended;
	struct MailNode *front ;
	struct MailNode *rear ;
};
struct MailBox** mbList = NULL ; 
struct SuspendContext ** susContext;
int initialize = 0 ; 

//[MAX_MAILBOXES];


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
            mb_id_pool = (mb_id_pool | 1<<i);
            printf("Free mb_id_pool = %x ",mb_id_pool);
            num_active_mailboxes++;
            return i;
        } 
    }
    printf("get_free_id Returning failure \n");
    return -1;
}
int set_free_id(int mb_id) {
   if((mb_id_pool & 1<<mb_id) == 0 ) {
        printf("ERROR :Freeing already freed mailbox \n");
        return -1;
   } else {
        mb_id_pool = mb_id_pool ^ (1<<mb_id); 
        return 0;
   }
}
int get_mb_id(int rx_pid) { 
    int i;
    for(i=0;i<MAX_MAILBOXES;i++) { 
        if(mbList[i]->owner_pid == rx_pid) {
            return i;
        }
    }
    return -1;
}
int get_sender_local_id(int sender_pid) { 
    int ret_val;
    int i;
    ret_val = get_mb_id(sender_pid);
    if(ret_val == -1) {
        for(i=0;i<MAX_SENDERS * MAX_MAILBOXES;i++) { 
            if(senderDB[i] == sender_pid) { 
                return i ;
            }
        }
    }else{
        return ret_val;
    }
   return -1;
}
int set_sender_local_id(int sender_pid) {
     int ret_val;
     int i;
     ret_val = get_mb_id(sender_pid);
     if(ret_val == -1 ) {
         for(i=MAX_MAILBOXES;i< MAX_SENDERS* MAX_MAILBOXES ; i++) {
            if(senderDB[i] == -1) {
                // This is the place for having mapping sender  pid 
                senderDB[i] = sender_pid;
                return i;
            }
         }
     }else{
        return ret_val;
     }
     return -1;
}
int addMailBox(struct MailBox *mb) {
    int mb_id;
    printf ("Entered addMailbox function\n");
	mb_id     = get_free_id();
	printf ("mb_id = %d\n", mb_id );
    if(mb_id < 0) {
        printf("ERROR : mailbox space full \n");
        return -1;
    }else{
       mbList[mb_id] = mb;
    }
    printf ("mb_id = %d\n", mb_id );
    return mb_id;
}
int create_mailbox(int owner_pid,int owner_uid,int owner_gid,int permissions)
{
    int i;
    struct MailBox *mb;
    printf ("Entered Create_mailbox function\n");
    mb = (struct Mailbox *) malloc(sizeof(struct MailBox));
    mb->owner_pid = owner_pid ;
    mb->owner_uid = owner_uid ;
    mb->owner_gid = owner_gid ;
	mb->perm      = permissions;
    for(i=0;i<MAX_SENDERS;i++) { 
        mb->senders[i] = -1 ;
        mb->suspended_sender[i] = -1 ; 
    }
    mb->num_senders = 0 ;
    mb->num_messages = 0 ;
    mb->sus_sender_idx = 0;
    mb->rx_suspended = 0;
    printf ("Before addMailbox function\n");
    mb->mb_id = addMailBox(mb);
    printf ("Before exiting from create_mailbox..... mb_id = %d\n", mb->mb_id);
    return mb->mb_id;
}
int putMsg(struct MailBox *m, struct  Message_mb * mail)
{
    struct MailNode *new_node ;
    if(m->num_messages >= MB_CAPACITY) {
        printf("Mailbox Full\n");
        return -1;
    }
    new_node = (struct MailNode *)(malloc(sizeof(struct MailNode)));
    new_node->next = NULL ;
	new_node->msg = mail;
    if(m->front == NULL)
    {    
		m->front = (struct MailNode *)new_node;
		m->rear  = (struct MailNode *)new_node;
    }
    else
    {
       new_node->next =(struct  MailNode *)m->rear;
	   m->rear = (struct MailNode *)new_node;
    }
    m->num_messages++; 
    return 0;
}

int getMsg(struct MailBox *mb,struct Message_mb *msg)
{
	struct  MailNode *prev=NULL;
    struct  MailNode *cur= (struct  MailNode *)mb->rear;
//	struct  Message msg;
    int debug_en = 1 ;

	if (mb->front == NULL) {
   	    if(debug_en) printf("\nCannot Delete. Queue is empty \n");		
        return -1;
  	} else if (cur->next==NULL) {
		msg = cur->msg;
		free(mb->front);
		mb->front=NULL;
		mb->rear=NULL;
	}else{
		while(cur!=(struct  MailNode *)mb->front && cur->next !=NULL){
			prev = cur;
			cur = cur->next;
			if(debug_en) printf("%s -> " , prev->msg->data);
		}
		msg =cur->msg;
		free(mb->front);
		prev->next=NULL;
		mb->front=( struct  MailNode *)prev;
	}
    mb->num_messages--; 
	return 0;
}




int isValidSender(int dest_id,int sender_pid) { 
   int i;
    for(i=0;i<mbList[dest_id]->num_senders;i++){ 
         if(mbList[dest_id]->senders[i] == sender_pid) {
            return 1;
         }
    }   
   printf("Sender Not found \n");
   return 0;
}
// Sys call functions 


PUBLIC int do_deposit()
{
   int MAX_DEST = 10 ; 
   int my_pid;
   int *dest;
   int i;
   int tx_pid;
   int tx_uid;
   int tx_gid;
   int dest_id;
   int sus_dests[MAX_SENDERS];
   int suspend_sender;
   struct Message_mb *msg;
   dest = (int *) malloc(MAX_DEST * sizeof(int) );
   msg = (struct Message_mb *) malloc(sizeof(struct Message_mb));
   my_pid = getpid();
   printf("Do deposit called Pid = %d my_pid = %d \n",m_in.m7_i1,my_pid);
   lprint("Do deposit called lp\n");
    tx_pid = m_in.m7_i1;
    tx_uid = m_in.m7_i2;
    tx_gid = m_in.m7_i3;
   sys_datacopy(tx_pid,m_in.m7_p2,VFS_PROC_NR,dest,MAX_DEST);
   sys_datacopy(tx_pid,m_in.m7_p1,VFS_PROC_NR,msg->data,256);
   msg->sender_id = tx_pid;
   suspend_sender = 0; 
   for(i=0;i< MAX_DEST;i++ ){
       int put_msg_ret_val;
       if(dest[i] != NULL) {
           if( dest[i] < 0 ) { 
                printf("Sender trying to send to %d \n",dest[i]);
                break; 
           }else{
                // Send message to this guy
               dest_id =  get_mb_id(dest[i]);
               if(dest_id >= 0 ) { 
                    // Check authorization 
                    if(isValidSender(dest_id,tx_pid)){
                    // Send message only when destination seems valid
                        put_msg_ret_val= putMsg(mbList[dest_id], msg);
                        if(put_msg_ret_val == -1) {
                            // Mailbox is full, the task needs to be blocked
                            // Add to the mailbox's blocked list
                            mbList[dest_id]->suspended_sender[ mbList[dest_id]->sus_sender_idx++] = tx_pid;
                            sus_dests[suspend_sender] = dest_id;
                            suspend_sender++; 
                      }
                    }
               }
           }
       }else{
            break;
       } 
   } 
   if(suspend_sender >= 1 ) { 
      // Procedure to suspend 
      // Update suspended list 
      struct SuspendContext * cntx;
      int this_local_id;
      this_local_id = get_sender_local_id(tx_pid);
      cntx = (struct SuspendContext *) malloc(sizeof(struct SuspendContext));
      cntx->msg = msg ; 
      cntx->suspened_for = (int *) malloc(sizeof(int) * suspend_sender);
      for(i=0;i<suspend_sender;i++) { 
        cntx->suspened_for[i] = sus_dests[i]; 
      } 
      cntx->len_sus_for = suspend_sender;
      susContext[this_local_id] = cntx;
   } else { 
      return 0;
   }
}

PUBLIC int do_retrieve() 
{
    int mb_id;
    int rx_pid;
    int ret_get_msg;
    struct Message_mb *msg;
    msg = (struct Message_mb *) malloc(sizeof(struct Message_mb));
    rx_pid = m_in.m7_i1;
    mb_id = get_mb_id(rx_pid);
    if(mb_id < 0) {
        printf("Invalid user may not have created Mailbox\n");
        return -1;
    } 
    ret_get_msg = getMsg(mbList[mb_id],msg);
    if(ret_get_msg < 0) { 
        printf("MailBox Empty \n");
        // The process should be suspended
        mbList[mb_id]->rx_suspended = 1 ; 
    }
    //sys_datacopy(VFS_PROC_NR,msg->data,rx_pid,m_in.m7_p1,strlen(msg->data));
    sys_datacopy(VFS_PROC_NR,msg->data,m_in.m_source,m_in.m7_p1,strlen(msg->data));
    return 0;
}
PUBLIC int do_destroy_mailbox() 
{
    int rx_pid;
    int del_mb_id;
    int ret_val = 0 ;
    struct MailBox *temp_mb;
    rx_pid    = m_in.m7_i1;
    del_mb_id = m_in.m7_i4;
    if((del_mb_id < MAX_MAILBOXES ) && (mbList[del_mb_id] != NULL) ) { 
        if(mbList[del_mb_id]->owner_pid == rx_pid) {
            // You are authorized to delete 
           temp_mb = mbList[del_mb_id];
           mbList[del_mb_id] = NULL;
           free(temp_mb);
           ret_val = set_free_id(del_mb_id);
        }else {
            printf("ERROR Unauthorized process = %d trying to delete Mb", rx_pid);
            ret_val = -1;
        } 
    } 
     
    return ret_val;
}
PUBLIC int do_create_mailbox()
{
    int rx_pid;
    int rx_uid;
    int rx_gid;
    int mb_id;
    int perm;
    int i;
    rx_pid = m_in.m7_i1;
    rx_uid = m_in.m7_i2;
    rx_gid = m_in.m7_i3;
    perm   = m_in.m7_i4;
    if(initialize  != 50 ) { 
       initialize = 50 ;
       mbList = (struct MailBox **) malloc(MAX_MAILBOXES*sizeof(struct MailBox * ));
       susContext=(struct SuspendContext **) malloc(MAX_MAILBOXES * MAX_SENDERS * sizeof(struct SuspendContext *));
      for(i=0;i<MAX_MAILBOXES * MAX_SENDERS ; i++) { 
          susContext[i] = NULL;
      }

    }
    mb_id = create_mailbox(rx_pid,rx_uid,rx_gid,perm);
    return mb_id;
}
PUBLIC int do_get_av_mailboxes()
{
    printf ("\n\nEntered do_get_av_mailboxes function\n");
    int *mailingList;
    int i;
    int tx_pid ;
    int mb_idx;
    int tx_uid;
    int tx_gid;
    mailingList = (int * ) malloc (num_active_mailboxes*2*sizeof(int));
    tx_pid = m_in.m7_i1;
    tx_uid = m_in.m7_i2;
    tx_gid = m_in.m7_i3;
    mb_idx = 0 ;
    for(i=0;i<MAX_MAILBOXES;i++) {
        if(mbList[i] != NULL) {
	//printf ("\n mbList [ %d ] not NULL\n", i);
                if((mbList[i]->perm & EVERYONE) || (( mbList[i]->perm & GROUP ) && mbList[i]->owner_gid == tx_gid ) || ((mbList[i]->perm & USER) && mbList[i]->owner_uid == tx_uid)) { 
                    mailingList[2*mb_idx] =mbList[i]->mb_id; 
                    mailingList[2*mb_idx+1] =mbList[i]->owner_pid; 
		     printf("get_av: list[%d] = %d list[%d] = %d ",mb_idx,mailingList[2*mb_idx],mb_idx+1,mailingList[2*mb_idx+1]);
                    mb_idx++;
                }
         }
    }
    
    //sys_datacopy(VFS_PROC_NR,mailingList,tx_pid,m_in.m7_p1,2*mb_idx);
     sys_datacopy(VFS_PROC_NR,mailingList,m_in.m_source,m_in.m7_p1,2*mb_idx);
     	   
    free(mailingList);
    return 0;
}
PUBLIC int do_register_mb() 
{  
    int tx_pid ;
    int mb_idx;
    int tx_uid;
    int tx_gid;
    int i;
    printf ("\n\nEntered do_register_mb function\n"); 
    tx_pid = m_in.m7_i1;
    tx_uid = m_in.m7_i2;
    tx_gid = m_in.m7_i3;
    mb_idx = m_in.m7_i4;
    if((mbList[mb_idx]->perm & EVERYONE) || (( mbList[mb_idx]->perm & GROUP ) && mbList[mb_idx]->owner_gid == tx_gid ) || ((mbList[mb_idx]->perm & USER) && mbList[mb_idx]->owner_uid == tx_uid)) { 
        // You are authorized to register 
	printf ("\n\n --- AUTHORIZED TO REGISTER -- \n"); 
        for(i=0;i<MAX_SENDERS;i++){
            if(mbList[mb_idx]->senders[i] == tx_pid) {
                printf("ERROR : Sender already registerd \n");
		return -2;
            }else if(mbList[mb_idx]->senders[i] == -1) { 
                // This is right place to add the sender
		printf ("\n\n NEW Entry created to register\n"); 
                mbList[mb_idx]->senders[i] = tx_pid;
                mbList[mb_idx]->num_senders++;
                set_sender_local_id(tx_pid);
		return 0;
            }
        }   
    }
    return -1;
}

PUBLIC int do_get_senders()
{
    int rx_pid;
    int mb_id;
    int *senderList;
    int i;
    int idx;
    struct MailBox *temp_mb;
    senderList = (int *) malloc (MAX_SENDERS * sizeof(int));
    mb_id = m_in.m7_i4;
    temp_mb = (struct MailBox *) mbList[mb_id];
    if((mb_id < MAX_MAILBOXES ) && (mbList[mb_id] != NULL) ) { 
        if(mbList[mb_id]->owner_pid == rx_pid) {
            // You are authorized
            for(i=0,idx=0;i<MAX_SENDERS;i++) { 
               if(mbList[mb_id]->senders[i] >= 0 ) {
                    senderList[idx++] = mbList[mb_id]->senders[i]; 
               } 
            }
            //sys_datacopy(VFS_PROC_NR,senderList,rx_pid,m_in.m7_p1,idx);
	    sys_datacopy(VFS_PROC_NR,senderList,m_in.m_source,m_in.m7_p1,idx);
            return 0;
        }else{
            printf("ERROR Unauthorized process %d trying to do get_sender\n",rx_pid);
            return -1;
        }
   }
    return 0;
}


