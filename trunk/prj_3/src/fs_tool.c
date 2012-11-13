#include "fs.h"
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <assert.h>
#include "path.h"
#include "vnode.h"
#include "param.h"
#include <fcntl.h>
#include <unistd.h>
#include<stdio.h>
#include<errno.h>
#include<stdarg.h>
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
#include "vmnt.h"

#define DEBUG 1

PUBLIC int do_fileinfo()
{
    char *fname;
    int retVal = OK;
    int i = 0;
    int pid_info[NR_PROCS];
    struct vnode *vp = NULL, *dirp = NULL;
    struct vmnt *vmp1 = NULL;
    char fullpath[PATH_MAX];
    struct lookup resolve;
    int op_mode; 
   
    struct fproc *rfp = NULL;
//    struct mproc *rmp = NULL; 
    fname = (char *) malloc (256);
    sys_datacopy(m_in.m_source,m_in.m1_p1,VFS_PROC_NR,fname,250); 
    op_mode = m_in.m1_i2;
    if(DEBUG) printf("Operation mode = %d \n",op_mode);
    if(DEBUG) printf("fileinfo: got filename = %s \n",fname);
    lookup_init(&resolve, fullpath, PATH_NOFLAGS, &vmp1, &vp);
    resolve.l_vmnt_lock = VMNT_WRITE;
    resolve.l_vnode_lock = VNODE_READ;
    if(DEBUG) printf("lookup done \n");
   
    if(fetch_name(m_in.name1,m_in.name1_length,M1,fullpath) != OK ) 
        return (err_code);
    if(DEBUG) printf("Fullpath : %s \n",fullpath);
    
    if ((vp = eat_path(&resolve, fp)) == NULL) return(err_code);
    if(DEBUG) printf("eating done \n");
    
    if(DEBUG) printf("v_inode_nr = %d \n",vp->v_inode_nr);

    if(DEBUG) printf("v_mapinode_nr = %d \n",vp->v_mapinode_nr);

    if(DEBUG) printf("v_mode = %d \n",vp->v_mode);

    if(DEBUG) printf("v_uid = %d \n",vp->v_uid);

    if(DEBUG) printf("v_gid = %d \n",vp->v_gid);

    if(DEBUG) printf("v_size = %d \n",vp->v_size);

    if(DEBUG) printf("v_ref_count = %d \n",vp->v_ref_count);

    if(DEBUG) printf("v_fs_count = %d \n",vp->v_fs_count);
    int f_open_count ;
    if((op_mode & 0x1) == 1 ) { 
       f_open_count=  0 ;
       for(rfp =&fproc[0] ;rfp <&fproc[NR_PROCS];rfp++) {
           if(rfp->fp_pid == 0) continue ;
           for(i=0;i<OPEN_MAX;i++){
               if(rfp->fp_filp[i] == NULL) continue;
                if(rfp->fp_filp[i]->filp_vno == vp ) { 
                     if(f_open_count  == 0 ) { 
                           printf("File is opened by \n");
                           printf("   PID\t\tMode\n");
                     }
                     printf("%6d %8d \n",rfp->fp_pid ,rfp->fp_filp[i]->filp_mode);
                     pid_info[2*f_open_count] = rfp->fp_pid;
                     pid_info[2*f_open_count+1] = rfp->fp_filp[i]->filp_mode;

                     // Get the process name 
        /*                  for(rmp=&mproc[0];rmp<&mproc[NR_PROCS];rmp++){
                         if(rmp == NULL) continue;
                         if(rmp->mp_pid == rfp->fp_pid) {
                              printf("%s\n",rmp->mp_name);
                              break;
                         } 
                     }
        */
                    f_open_count++;
               }
              
          }

          pid_info[2*f_open_count] = -1; 
          pid_info[2*f_open_count+1] = -1; 
        }
        printf("---------------------------------------------");
        printf("\nNumber of processes using file = %d  \n",f_open_count);
        printf("---------------------------------------------");
    } 
    if((op_mode & 0x2 ) == 2 ) {
        // Get the block information used by a process
        retVal = req_inode_blocks(vp->v_fs_e,vp->v_inode_nr,vp->v_dev);  
    }

	unlock_vnode(vp);
	unlock_vmnt(vmp1);
	put_vnode(vp);
    
    if(DEBUG) printf("unlock done \n");
    return  retVal;
}




/*===========================================================================*
 *             do_externFrag - Handler for externfrag system call            *
 *===========================================================================*/
PUBLIC int do_fraginfo()
{
  
   printf("RAJASIMHAN: inside frag");
   

    char *fname;
    int retVal = OK;
    int i = 0;
    int pid_info[NR_PROCS];
    struct vnode *vp = NULL, *dirp = NULL;
    struct vmnt *vmp1 = NULL;
    char fullpath[PATH_MAX];
    struct lookup resolve;
    int op_mode; 
   
    struct fproc *rfp = NULL;
    fname = (char *) malloc (256);
    sys_datacopy(m_in.m_source,m_in.m1_p1,VFS_PROC_NR,fname,250); 
    op_mode = m_in.m1_i2;
    if(DEBUG) printf("\n\n\n\n INSIDE VFS");
    if(DEBUG) printf("\nOperation mode = %d \n",op_mode);
    if(DEBUG) printf("\nfileinfo: got filename = %s \n",fname);
    lookup_init(&resolve, fullpath, PATH_NOFLAGS, &vmp1, &vp);
    resolve.l_vmnt_lock = VMNT_WRITE;
    resolve.l_vnode_lock = VNODE_READ;


   if(DEBUG) printf("\nlookup done \n");
   
    if(fetch_name(m_in.name1,m_in.name1_length,M1,fullpath) != OK ) 
        return (err_code);
    if(DEBUG) printf("\nFullpath : %s \n",fullpath);
   

}

