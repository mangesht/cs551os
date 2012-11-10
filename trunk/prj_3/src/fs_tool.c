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

PUBLIC int do_fileinfo()
{
    char *fname;
    fname = (char *) malloc (256);
    sys_datacopy(m_in.m_source,m_in.m1_p1,VFS_PROC_NR,fname,256); 
    printf("fileinfo: got filename = %s ",fname);

    return 0;
}
