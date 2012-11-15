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
 *				PROJECT 3      *
 *===========================================================================*/
#define TRUE 1
#define FALSE 0
#define DEBUG 0

int blocksPerZone = 0;
int EOF_reached = FALSE;
int block_count = 0;
int scale = 0;

int FraginFS(struct super_block *sp);
int FragInInode(struct super_block *sp, u32_t inode_no);


/* Handler for message REQ_EXTFRAG */
PUBLIC int fs_getfrag()
{
    struct super_block *sp = NULL;
    if(DEBUG==1) printf("MFS: In mfs_extfragInfo \n");
     sp = get_super(fs_dev);

     /* Update the blocks per zone global */
     blocksPerZone = 1 << (sp->s_log_zone_size);
     FraginFS(sp);
     return 1;
}


/* To compute the degree of fragmentation in the file system */
int FraginFS(struct super_block *sp)
{
    u32_t inodesTotal   = 0; 
    u32_t inodesScanned = 0; 

    block_t start_block;				/* first bit block */
    block_t block;                      /* Start from Zero block */
    bit_t   map_bits;		        	/* how many bits are there in the bit map? */
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

    u32_t max_efrag = 0; 
    u32_t min_efrag = 0; 
    u32_t max_ifrag = 0; 
    u32_t min_ifrag = 0; 
    u32_t avg_ifrag = 0;

    unsigned word, bcount;

    if(DEBUG == 1)printf("MFS: In computeExtFrag\n");
	
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

	if(DEBUG==1 )printf("Inode bitmap Block #%d\n",block);
	if(DEBUG==1 )printf("=========================\n");

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

 					 printf("\n Block and Zone Information");
	  	                      printf("\n ----------------------------------------------------------------\n");
					 printf("Blocks per zone             = %d\n",blocksPerZone);
					 printf("Block size                  = %d bytes\n",sp->s_block_size);					 
					 if(DEBUG == 1) printf("inodes Scanned              = %ld\n",inodesScanned);
					 printf("Total Zones in File System   = %ld\n",zone_map_bits);
					 printf("Total Blocks In File System  = %ld\n\n",TotalBlocksInFS);
                    			 printf("Total Fragmentation Blocks   = %ld\n",TotalFragBlocks);
	
					 /* X 100 to express in percentage */
					 extFrag = ((float)TotalFragBlocks / TotalBlocksInFS)*100;

					 printf("\nExternal fragmentation  (in %) = %f",extFrag);
		           	        printf("\n----------------------------------------------------------------\n");
					 printf("\nLargest fragmentation = %ld",max_efrag);
					 printf("\nLeast   fragmentation = %ld",min_efrag);
					 printf("\nAverage fragmentation = %ld\n",TotalFragBlocks / TotalBlocksInFS);
					 
					 printf("\Internal fragmentation  (in %) = %f",extFrag);
					 printf("\n----------------------------------------------------------------\n");
					 printf("\nLargest fragmentation = %ld",max_ifrag);
					 printf("\nLeast   fragmentation = %ld",min_ifrag);
					 printf("\nAverage fragmentation = %ld",0);


					 if(DEBUG == 1)
					 {
					 printf("MFS: check: Total number of inodes = %ld\n",sp->s_ninodes);
				    	 printf("MFS: check: bit_blocks = %d, map bits=%ld\n",bit_blocks, map_bits);
					 }
		             return(OK); 
               }
			int mm=1;
		   /* Add to total fragmentation by counting fragmentation in inodes */
		   if(((k & (1 << i)) != 0) && (inodesScanned != 0)) 
		   {
		       if(DEBUG == 1)printf("MFS: checking  fragmentation in inodenumber (bit number #%ld)%ld\n",inodesScanned,b);
		       tmpFragBlocks = FragInInode(sp, inodesScanned);
              	 TotalFragBlocks += tmpFragBlocks;

			   if (tmpFragBlocks > max_efrag)max_efrag=tmpFragBlocks;
			   if (tmpFragBlocks < min_efrag)min_efrag=tmpFragBlocks;
	
		   }
 		   inodesScanned++;
	     }
	}
    block++;
      
    }while (--bcount > 0);
    return(OK); 
}


/* To compute fragmentation blocks in each inode */
int FragInInode(struct super_block *sp, u32_t inode_no)
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

    if(blocks_in_last_zone != 0)fragment_blocks = blocksPerZone - blocks_in_last_zone; 
    if(DEBUG == 1)printf("For inode=%d, file size = %ld, total_blocks_used = %d, fragment_blocks = %d\n",inode_no,rip->i_size,total_blocks_used,fragment_blocks);
        
    return(fragment_blocks);
}

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
/*===========================================================================*
 *		Calculating degree of fragmentation - END  		     *
 *===========================================================================*/

/*===========================================================================*
 *	 Procedures for displaying disk blocks of a file - START	     *
 *===========================================================================*/
int get_dbl_ind_zone(u32_t DoubleInd_zone_num, struct inode *ip);
int get_sin_ind_zone(u32_t singleInd_zone_num, struct inode *ip);
int get_dir_zone(u32_t zone_num, struct inode *ip);

void find_int_frag(dev_t fs_dev, struct inode *ip,struct super_block *sp) ;
void get_frag_for_dir_zone(u32_t zone_num,dev_t fs_dev, struct inode *ip,
     struct super_block *sp,int *blk_cnt) ;
#define NUM_FRAG_BINS 8
int tot_int_frag;
int num_int_frag_entries;
int min_frag_val ;
int max_frag_val;
int num_dirs;
int frag_stat[NUM_FRAG_BINS];

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

PUBLIC int fs_get_int_frag()
{
    dev_t dev;
    ino_t in;

    struct inode *ip = NULL;
    struct super_block *sp = NULL;
    int total_blocks_used = 0;
    long tot_blks;
    int inf_frag ;
    int i = 0;
 
    block_count = 0;
    
    dev = (dev_t) fs_m_in.REQ_DEV;
    in = (ino_t) fs_m_in.REQ_INODE_NR;
    inf_frag = (int) fs_m_in.REQ_COUNT;
    sp = get_super(fs_dev);
    ip = get_inode(fs_dev, in);
    if(DEBUG) printf("Message int REQ_COUNT = %d \n",fs_m_in.REQ_COUNT);    


    printf("\n"); 
    if(DEBUG == 1)
    {
	    printf("MFS: Retrieve inode number : %d\n", in);
    	printf("MFS: Scale : %d\n",sp->s_log_zone_size);
    }
    if(DEBUG) printf(" File mode = %x \n",ip->i_mode);
     /* Update the blocks per zone global */
    blocksPerZone = 1 << (sp->s_log_zone_size);
    total_blocks_used  = ceil((double)ip->i_size/(sp->s_block_size));
    /* Update the global data */
    block_count = total_blocks_used;
    scale       = sp->s_log_zone_size;
    EOF_reached = FALSE; 
    /* If no blocks are used by the file: Filesize = 0 */
    if(block_count==0) {
       EOF_reached = TRUE;
       printf("\n\n++ No data disk blocks used by file\n");
    }
    if(DEBUG) printf("Internal fragment selected inode = %d \n",in);
    //--------------------------------------------------
    //  Initialization of statistic variables ----------
    //--------------------------------------------------
    tot_int_frag = 0 ; 
    num_int_frag_entries = 0 ;
    min_frag_val = sp->s_block_size;
    max_frag_val = 0;
    num_dirs = 0; 
    for(i=0;i<NUM_FRAG_BINS;i++) { 
        frag_stat[i] = 0;
    }
    find_int_frag(dev,ip,sp);
    printf("Internal fragmentation details \n");
    printf("Number of Files               = %ld \n",num_int_frag_entries);
    printf("Number of Dirs                = %ld \n",num_dirs);
    printf("Total internal fragmentation  = %ld  \n",tot_int_frag );
    printf("Min internal fragmentation    = %ld  \n",min_frag_val );
    printf("Max internal fragmentation    = %ld  \n",max_frag_val );
    printf("Avg internal fragmentation    = %ld  \n",tot_int_frag/num_int_frag_entries );
   printf("------------------------------------------");
   printf(" Distribution of fragmented data \n");
   for(i=0;i<NUM_FRAG_BINS;i++){
        printf("Bin %4d - %4d \t -> %d",(i*sp->s_block_size / NUM_FRAG_BINS), 
            ((i+1)*sp->s_block_size / NUM_FRAG_BINS),
            frag_stat[i]);
   printf("------------------------------------------");
   }
   
    
    printf("Total internal frag = %d \n",tot_int_frag );
    put_inode(ip);

    if(DEBUG == 1)
	printf("MFS: ***** END *****\n");


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
    int inf_frag ;
    int i = 0;
 
    block_count = 0;
    
    dev = (dev_t) fs_m_in.REQ_DEV;
    in = (ino_t) fs_m_in.REQ_INODE_NR;
    sp = get_super(fs_dev);
    ip = get_inode(fs_dev, in);
    if(DEBUG) printf("Message int REQ_COUNT = %d \n",fs_m_in.REQ_COUNT);    


    printf("\n"); 
    if(DEBUG == 1)
    {
	    printf("MFS: Retrieve inode number : %d\n", in);
    	printf("MFS: Scale : %d\n",sp->s_log_zone_size);
    }
    printf(" File mode = %x \n",ip->i_mode);
     /* Update the blocks per zone global */
    blocksPerZone = 1 << (sp->s_log_zone_size);
    total_blocks_used  = ceil((double)ip->i_size/(sp->s_block_size));
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
void find_int_frag(dev_t fs_dev, struct inode *ip,struct super_block *sp) {
    int i;
    int blk_cnt = 0;
    int total_blocks_used;
    int int_frag_val; 
    int used_bytes;
    static int times = 0 ;
    times++;
  //  if(times > 10 ){
  //     printf("ERROR:Exceed return \n");
 //      return;
 //   }
    if(DEBUG) printf("Inside find_int_frag i_num = %d \n",ip->i_num);
    total_blocks_used  = ceil((double)ip->i_size/(sp->s_block_size));
    blk_cnt = total_blocks_used ;
    if((ip->i_mode & I_TYPE) != I_DIRECTORY) {
       // This is a file
       if(DEBUG) printf("This is file ");
       used_bytes = (ip->i_size % sp->s_block_size);
       int_frag_val = used_bytes == 0 ? 0 : sp->s_block_size - used_bytes ; 
       tot_int_frag += int_frag_val ; 
       min_frag_val = int_frag_val < min_frag_val ? int_frag_val : min_frag_val ; 
       max_frag_val = int_frag_val > max_frag_val ? int_frag_val : max_frag_val ; 

       num_int_frag_entries++;
       frag_stat[(int)((int_frag_val * NUM_FRAG_BINS) / sp->s_block_size)]++; 
       if(DEBUG) printf("Int frag = %d \n",int_frag_val );
    }else{
    for(i=0; (i<ip->i_ndzones) && blk_cnt>0; i++) { 
       if(DEBUG) printf("Direct zone number = %d \n",i);

       used_bytes = (ip->i_size % sp->s_block_size);
       int_frag_val = used_bytes == 0 ? 0 : sp->s_block_size - used_bytes ; 
       tot_int_frag += int_frag_val ; 
       min_frag_val = int_frag_val < min_frag_val ? int_frag_val : min_frag_val ; 
       max_frag_val = int_frag_val > max_frag_val ? int_frag_val : max_frag_val ; 

       num_int_frag_entries++;
       num_dirs++;
       frag_stat[(int)((int_frag_val * NUM_FRAG_BINS) / sp->s_block_size)]++; 

        
        get_frag_for_dir_zone(ip->i_zone[i],fs_dev,ip,sp,&blk_cnt);
    }
   }

}
void get_frag_for_dir_zone(u32_t zone_num,dev_t fs_dev, struct inode *ip,
     struct super_block *sp,int *blk_cnt) {
     int i;
     int dc;
     u32_t block_num = 0;
     struct buf *bp; 
     struct direct *dp;
     struct inode *dir_inode;
     if(DEBUG) printf("Inside get_frag_for_dir_zone blk_count = %d \n",*blk_cnt);
     if(*blk_cnt == 0 ) { 
       return(OK); 
     }
     if(zone_num == NO_ZONE) { 
        return -1;
     }
      // Loop for number of blocks in a zone 
     for(i=0;(i< (1<<scale)) && *blk_cnt > 0 ;i++){
        block_num = (zone_num << scale + i);
        if(DEBUG) printf("Block_num + %d \n",block_num);
  	    bp = get_block(ip->i_dev, block_num, NORMAL); 
        assert(bp != NULL);
        printf("This block allocated only %d \n",bp->b_bytes);
        printf("It uses = %ld \n",ip->i_size);
        if(DEBUG) printf("This dir has %d entries \n",dp<&bp->b_dir[NR_DIR_ENTRIES(ip->i_sp->s_block_size)]);
        
        for(dc=0,dp=&bp->b_dir[0];dp<&bp->b_dir[NR_DIR_ENTRIES(ip->i_sp->s_block_size)]; dp++,dc++) { 
            if(dc < 2 ) { 
                if(DEBUG) printf("Dot or dot dot please check inode %d \n",dp->mfs_d_ino);
                continue;
            }
            if((dp->mfs_d_ino< 2) || dp->mfs_d_ino == ip->i_num) continue ;
 
            dir_inode = get_inode(fs_dev, dp->mfs_d_ino);
            if(DEBUG) printf("Doing rec call with inode num = %d = %d \n",dp->mfs_d_ino,dir_inode->i_num);
            find_int_frag(fs_dev,dir_inode,sp);   
        } 
        *blk_cnt= *blk_cnt -1; 
        if(DEBUG) printf("block count = %d ",*blk_cnt);
  	    put_block(bp, FULL_DATA_BLOCK);
     }
}
