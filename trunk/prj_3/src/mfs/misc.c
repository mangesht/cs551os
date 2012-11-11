#include "fs.h"
#include <assert.h>
#include <math.h>
#include <minix/vfsif.h>
#include <minix/bdev.h>
#include "inode.h"
#include "clean.h"
#include <string.h>
#include <minix/com.h>
#include <minix/u64.h>
#include "buf.h"
#include "super.h"
#include "const.h"


/*===========================================================================*
 *				fs_sync					     *
 *===========================================================================*/
PUBLIC int fs_sync()
{
/* Perform the sync() system call.  Flush all the tables. 
 * The order in which the various tables are flushed is critical.  The
 * blocks must be flushed last, since rw_inode() leaves its results in
 * the block cache.
 */
  struct inode *rip;
  struct buf *bp;

  assert(nr_bufs > 0);
  assert(buf);

  /* Write all the dirty inodes to the disk. */
  for(rip = &inode[0]; rip < &inode[NR_INODES]; rip++)
	  if(rip->i_count > 0 && IN_ISDIRTY(rip)) rw_inode(rip, WRITING);

  /* Write all the dirty blocks to the disk, one drive at a time. */
  for(bp = &buf[0]; bp < &buf[nr_bufs]; bp++)
	  if(bp->b_dev != NO_DEV && ISDIRTY(bp)) 
		  flushall(bp->b_dev);

  return(OK);		/* sync() can't fail */
}


/*===========================================================================*
 *				fs_flush				     *
 *===========================================================================*/
PUBLIC int fs_flush()
{
/* Flush the blocks of a device from the cache after writing any dirty blocks
 * to disk.
 */
  dev_t dev = (dev_t) fs_m_in.REQ_DEV;
  if(dev == fs_dev) return(EBUSY);
 
  flushall(dev);
  invalidate(dev);

  return(OK);
}


/*===========================================================================*
 *				fs_new_driver				     *
 *===========================================================================*/
PUBLIC int fs_new_driver(void)
{
/* Set a new driver endpoint for this device. */
  dev_t dev;
  cp_grant_id_t label_gid;
  size_t label_len;
  char label[sizeof(fs_dev_label)];
  int r;

  dev = (dev_t) fs_m_in.REQ_DEV;
  label_gid = (cp_grant_id_t) fs_m_in.REQ_GRANT;
  label_len = (size_t) fs_m_in.REQ_PATH_LEN;

  if (label_len > sizeof(label))
	return(EINVAL);

  r = sys_safecopyfrom(fs_m_in.m_source, label_gid, (vir_bytes) 0,
	(vir_bytes) label, label_len, D);

  if (r != OK) {
	printf("MFS: fs_new_driver safecopyfrom failed (%d)\n", r);
	return(EINVAL);
  }

  bdev_driver(dev, label);

  return(OK);
}


/*===========================================================================*
 *				PROJECT 3      *
 *===========================================================================*/


#define TRUE 1
#define FALSE 0
#define DEBUG 0 

int blocksPerZone = 0;
int EOF_reached = FALSE;
int block_count = 0;
int scale = 0;

int printSuperBlock(struct super_block *sp);
int computeExtFraginFS(struct super_block *sp);
int computeExtFragInInode(struct super_block *sp, u32_t inode_no);


/*===========================================================================*
 *				PROJECT 3 - Requirement 3 		     *
 *===========================================================================*/
int printSuperBlock(struct super_block *sp)
{
     printf("s_ninodes=%d \t s_nzones=%d \n", sp->s_ninodes,  sp->s_nzones);
     printf("s_imap_blocks=%d \t s_zmap_blocks=%d \n", sp->s_imap_blocks,  sp->s_zmap_blocks);
     printf("s_firstdatazone_old=%d \t s_log_zone_size=%d \n",  sp->s_firstdatazone_old ,  sp->s_log_zone_size);
     printf("s_max_size=%ld \t s_zones=%d \n", sp->s_max_size,  sp->s_zones);

     printf("s_block_size=%d \t s_inodes_per_block=%d \n", sp->s_block_size, sp->s_inodes_per_block);
     printf("s_firstdatazone=%d \t s_dev=%d \n", sp->s_firstdatazone,  sp->s_dev);
     printf("s_ndzones=%d \t s_nindirs=%d \n", sp->s_ndzones,  sp->s_nindirs);
     printf("s_isearch=%d \t s_zsearch=%d \n", sp->s_isearch,  sp->s_zsearch);
     printf("s_native=%d  \n", sp->s_native);
}

/* To compute fragmentation blocks in each inode */
int computeExtFragInInode(struct super_block *sp, u32_t inode_no)
{
    struct inode *rip;
    int total_blocks_used = 0;
    int blocks_in_last_zone = 0;
    int fragment_blocks = 0; 

    rip = get_inode(fs_dev, (ino_t)inode_no);

    if(!rip) {
	    printf("MFS: Error: Invalid case: find_inode failed should not come here \n");
	    return 0;
    }

    put_inode(rip);

    total_blocks_used   = (double) ceil((double)rip->i_size/(sp->s_block_size));
    blocks_in_last_zone = total_blocks_used % blocksPerZone;

    if(blocks_in_last_zone != 0)
	   fragment_blocks = blocksPerZone - blocks_in_last_zone; 

    printf("For inode=%d, file size = %ld, total_blocks_used = %d, fragment_blocks = %d\n",
	    inode_no,rip->i_size,total_blocks_used,fragment_blocks);
        
    return(fragment_blocks);
}

/* To compute the degree of fragmentation in the file system */
int computeExtFragInFS(struct super_block *sp)
{
    u32_t inodesTotal   = 0; 
    u32_t inodesScanned = 0; 

    block_t start_block;		/* first bit block */
    block_t block;                      /* Start from Zero block */
    bit_t   map_bits;		        /* how many bits are there in the bit map? */
    short   bit_blocks;	     	        /* how many blocks are there in the bit map? */

    struct buf *bp;
    bitchunk_t *wptr, *wlim, k;
    bit_t i, b;

    u32_t TotalFragBlocks = 0;
    u32_t tmpFragBlocks = 0;
    u32_t TotalBlocksInFS = 0; 
    bit_t zone_map_bits;	
    char str[500] = {0};
    float extFrag = 0.0;

    unsigned word, bcount;

    if(DEBUG == 1)
	    printf("MFS: In computeExtFrag\n");
	
    zone_map_bits =  (bit_t) (sp->s_zones - (sp->s_firstdatazone - 1));
    TotalBlocksInFS = zone_map_bits * blocksPerZone;

    /* Initialise to Max number of inodes in the system */
    inodesTotal = sp->s_ninodes;

    start_block = START_BLOCK;
    map_bits = (bit_t) (sp->s_ninodes + 1);
    bit_blocks = sp->s_imap_blocks;

    /* Start from beginning */
    block = 0;
    word =  0;
    
    /* Iterate over all blocks, because we start from the beginnnig and not in middle. */
    bcount = bit_blocks;
   
    do {

	bp = get_block(sp->s_dev, start_block + block, NORMAL);
	wlim = &bp->b_bitmap[FS_BITMAP_CHUNKS(sp->s_block_size)];

	printf("Inode bitmap Block #%d\n",block);
	printf("=========================\n");

	/* Iterate over the words in block. */
	for (wptr = &bp->b_bitmap[word]; wptr < wlim; wptr++) {

		k = (bitchunk_t) conv2(sp->s_native, (int) *wptr);

		/* Find allocated bit. */
		for (i = 0; i < FS_BITCHUNK_BITS; ++i)
	       	{
		   /* Bit number from the start of the bit map. */
	           b = ((bit_t) block * FS_BITS_PER_BLOCK(sp->s_block_size))
		           + (wptr - &bp->b_bitmap[0]) * FS_BITCHUNK_BITS
		           + i;

		   /* Break out of loop if current validated exceeded total number of bit map bits */
                   if (b >= map_bits)
                   {
                         printf("\n----------------------------------------------------------------\n");
			 printf("++ Blocks per zone             = %d\n",blocksPerZone);
			 printf("++ Block size                  = %d bytes\n",sp->s_block_size);
			 
			 if(DEBUG == 1)
			     printf("inodes Scanned              = %ld\n",inodesScanned);
			 printf("++ Total Zones in File System   = %ld\n",zone_map_bits);
			 printf("++ Total Blocks In File System  = %ld\n\n",TotalBlocksInFS);

                         printf("++ Total Fragmentation Blocks  = %ld\n",TotalFragBlocks);

			 /* X 100 to express in percentage */
			 extFrag = ((float)TotalFragBlocks / TotalBlocksInFS)*100;
			 sprintf(str, "%f", extFrag);

			 printf("\n++ DEGREE OF EXTERNAL FRAGMENTATION in File System (percentage) = %s \%\n", str);
                         printf("----------------------------------------------------------------\n");
			 

			 if(DEBUG == 1)
			 {
			     printf("MFS: check: Total number of inodes = %ld\n",sp->s_ninodes);
		    	     printf("MFS: check:  bit_blocks = %d, map bits=%ld\n",bit_blocks, map_bits);
			 }
                         return(OK); 
                   }

		   /* Check fragmentation in occupied inodes and add to total fragmentation */
		   if(((k & (1 << i)) != 0) && (inodesScanned != 0)) /* ignore bit 0 */
		   {
		       if(DEBUG == 1)
		          printf("MFS: Going to check for fragmentation for inode # (bit number #%ld)%ld\n",inodesScanned,b);
		       tmpFragBlocks = computeExtFragInInode(sp, inodesScanned);
                       TotalFragBlocks += tmpFragBlocks;
		   }
 		   inodesScanned++;
	        }
	}

      block++;
      
    }while (--bcount > 0);

    if(DEBUG == 1)
    {
      /* Safe prints. Will be removed later. These prints should not come in any setup */
      printf("MFS: should not come here check: inodesScanned=%ld\n",inodesScanned);
      printf("MFS: should not come here check: TotalFragBlocks=%ld\n",TotalFragBlocks);
      printf("MFS: should not come here check: Total number of inodes = %ld\n",sp->s_ninodes);
      printf("MFS: should not come here check:  bit_blocks = %d, map bits=%ld\n",bit_blocks, map_bits);
    }
    /*panic("b < map_bits");*/

    return(OK); 
}

/* Handler for message REQ_EXTFRAG */
PUBLIC int fs_getfrag()
{
     struct super_block *sp = NULL;

     if(DEBUG == 1)
	printf("MFS: In fs_extfragInfo \n");

     sp = get_super(fs_dev);

     /* Update the blocks per zone global */
     blocksPerZone = 1 << (sp->s_log_zone_size);

     computeExtFragInFS(sp);
	 
     if(DEBUG == 1)
         printSuperBlock(sp);

     return(OK); 
}
/*===========================================================================*
 *		Calculating degree of fragmentation - END  		     *
 *===========================================================================*/

/*===========================================================================*
 *	 Procedures for displaying disk blocks of a file - START	     *
 *===========================================================================*/
int get_dbl_ind_zone(u32_t DoubleInd_zone_num, struct inode *ip);
int get_sin_ind_zone(u32_t singleInd_zone_num, struct inode *ip);
int get_dir_zone(u32_t zone_num, struct inode *ip);


/* Read Double indirect Zone */
int get_dbl_ind_zone(u32_t DoubleInd_zone_num, struct inode *ip)
{
    struct buf *double_bp = NULL;
    u32_t DoubleInd_block_num = 0;
    int i = 0;
    u32_t single_indirect_zone_num = 0;

    if (DoubleInd_zone_num == NO_ZONE) 
    {
	if(DEBUG == 1)
		printf("get_dbl_ind_zone: Zone %d doesn't have any value. Return\n",DoubleInd_zone_num);
	  return -1;
    }	

    DoubleInd_block_num = DoubleInd_zone_num << scale;

    double_bp = get_block(ip->i_dev, DoubleInd_block_num, NORMAL); 

    for(i=0; (i<ip->i_nindirs) && !(EOF_reached) ; i++)
    {
       single_indirect_zone_num = rd_indir(double_bp, i);
       get_sin_ind_zone(single_indirect_zone_num, ip);
    }

    put_block(double_bp, INDIRECT_BLOCK);
    return (OK); 
}

/* Read single indirect Zone */
int get_sin_ind_zone(u32_t singleInd_zone_num, struct inode *ip)
{
    struct buf *single_bp = NULL;
    u32_t singleInd_block_num = 0;
    int i = 0;
    u32_t direct_zone_num = 0;

    if (singleInd_zone_num == NO_ZONE) 
    {
	if(DEBUG == 1)
		printf("get_sin_ind_zone: Zone %d doesn't have any value. Return\n",singleInd_zone_num);
	return -1; 
    }	

    singleInd_block_num = singleInd_zone_num << scale;

    single_bp = get_block(ip->i_dev, singleInd_block_num, NORMAL); 

    for(i=0; (i<ip->i_nindirs) && !(EOF_reached) ; i++)
    {
       direct_zone_num = rd_indir(single_bp, i);
       get_dir_zone(direct_zone_num, ip);
    }
  
    put_block(single_bp, INDIRECT_BLOCK);
    return (OK); 
}

/* Read direct Zone */
int get_dir_zone(u32_t zone_num, struct inode *ip)
{
    int i = 0;
    u32_t  block_num = 0;
    struct buf *bp;

    /* printf("Reading zone #%ld\n", zone_num); */
    
    if(block_count==0)
    {
	EOF_reached = TRUE;
	return (OK); 
    }

    if(zone_num == NO_ZONE)
    {
	if(DEBUG == 1)
		printf("get_dir_zone: Zone#%ld doesn't have any value. Return\n",zone_num);
	return -1; 
    }

    /* Loop till number of blocks in a zone */
    for(i=0;(i< (1<<scale)) && (block_count != 0);i++)
    {
  	block_num = (zone_num << scale) + i;
  	bp = get_block(ip->i_dev, block_num, NORMAL); 
    // Mangesh this can be optimized 

  	if(bp->b_dev == ip->i_dev)
	{
  	    printf("[Block # %ld] ",bp->b_blocknr);

	    /* Reduce the block count */
	    block_count--;

            /* If block count is zero, indicates EOF of the file, so set the flag */
	    if(block_count==0) 
		EOF_reached = TRUE;
  	}
	else
	{
	   /*panic("bp->b_dev != ip->i_dev");*/
	}

  	put_block(bp, FULL_DATA_BLOCK);
    }

    return (OK);
}

/* Handler for message REQ_GETINODEBLOCKS */
PUBLIC int fs_get_inode_blocks()
{
    dev_t dev;
    ino_t in;

    struct inode *ip = NULL;
    struct super_block *sp = NULL;
    int total_blocks_used = 0;
    long tot_blks;
    int i = 0;
 
    block_count = 0;
    
    dev = (dev_t) fs_m_in.REQ_DEV;
    in = (ino_t) fs_m_in.REQ_INODE_NR;

    sp = get_super(fs_dev);
    ip = get_inode(fs_dev, in);
    printf("\n"); 
    if(DEBUG == 1)
    {
	    printf("MFS: Retrieve inode number : %d\n", in);
    	printf("MFS: Scale : %d\n",sp->s_log_zone_size);
    }
     /* Update the blocks per zone global */
    blocksPerZone = 1 << (sp->s_log_zone_size);

     total_blocks_used  = ceil((double)ip->i_size/(sp->s_block_size));
   //   total_blocks_used = ceil(tot_blks) ; 

    /* Update the global data */
    block_count = total_blocks_used;
    scale       = sp->s_log_zone_size;
    EOF_reached = FALSE; 

    /* If no blocks are used by the file: Filesize = 0 */
    if(block_count==0) {
	    EOF_reached = TRUE;
        printf("\n\n++ No data disk blocks used by file\n");
    }
    
    for(i=0; (i<ip->i_ndzones) && !(EOF_reached) ; i++)
    {
       get_dir_zone(ip->i_zone[i], ip);
    }
	
    if(EOF_reached)
    { 
	if(DEBUG == 1) 
	     printf("MFS: EOF reached after direct zones in the inode itself\n");
    }
    else
    { 
        get_sin_ind_zone(ip->i_zone[ip->i_ndzones], ip);
        if(EOF_reached)
	{ 
   	   if(DEBUG == 1)
	      printf("MFS: EOF reached after single indirect zones\n");
	}
	else
	{
	   get_dbl_ind_zone(ip->i_zone[ip->i_ndzones+1], ip);

	   if(EOF_reached)
	   {  
	       if(DEBUG == 1) printf("MFS: EOF reached after double indirect zones\n");
	   }
	}
    }

    printf("\n----------------------------------------------------------------\n");
    printf("++ Block size                 : %ld bytes\n",sp->s_block_size);
    printf("++ Blocks per zone            : %d\n\n",blocksPerZone);
    printf("++ Size of the given file     : %ld bytes\n\n",ip->i_size);
    printf("++ Total data disk blocks     : %d\n",total_blocks_used);
    printf("----------------------------------------------------------------\n");
    

    put_inode(ip);

    if(DEBUG == 1)
	printf("MFS: ***** END *****\n");

    return (OK);
}

