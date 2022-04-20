/*********** inode.c file **********/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#define BLKSIZE 1024
typedef struct ext2_group_desc GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;
SUPER *sp;
GD *gp;
INODE *ip;
DIR *dp;
char buf[BLKSIZE];
int fd, firstdata, inodesize, blksize, iblock;
char *dev = "/dev/sdb2"; // default device name
int get_block(int fd, int blk, char *buf){
	lseek(fd, blk*BLKSIZE, SEEK_SET);
	return read(fd, buf, BLKSIZE);
}
int inode(char *dev){
	int i;
	fd = open(dev, O_RDONLY);
	if (fd < 0){
		printf("open failed\n"); exit(1);
	}
	get_block(fd, 1, buf); // get superblock
	sp = (SUPER *)buf;
	firstdata = sp->s_first_data_block;
	inodesize = sp->s_inode_size;
	blksize = 1024*(1<<sp->s_log_block_size);
	printf("first_data_block=%d block_size=%d inode_size=%d\n",firstdata, blksize, inodesize);
	get_block(fd, (firstdata+1), buf); // get group descriptor
	gp = (GD *)buf;
	printf("bmap_block=%d imap_block=%d inodes_table=%d\n",
		gp->bg_block_bitmap,
		gp->bg_inode_bitmap,
		gp->bg_inode_table);
	printf("free_blocks_count=%d free_inodes_count=%d used_dir_count=%d\n",
		gp->bg_free_inodes_count,
		gp->bg_free_inodes_count,
		gp->bg_used_dirs_count);
	iblock = gp->bg_inode_table;
	printf("root inode information:\n", iblock);
	printf("---------------------–\n");
	get_block(fd, iblock, buf);
	ip = (INODE *)buf + 1; // ip point at #2 INODE
	printf("mode=%4x ", ip->i_mode);
	printf("uid=%d gid=%d\n", ip->i_uid, ip->i_gid);
	printf("size=%d\n", ip->i_size);
	printf("links=%d\n", ip->i_links_count);
	for (i=0; i<15; i++){ // print disk block numbers
		if (ip->i_block[i]) // print non-zero blocks only
			printf("i_block[%d]=%d\n", i, ip->i_block[i]);
	} 
}
int main(int argc, char *argv[ ]){
	if (argc>1) dev = argv[1];
		inode(dev);
}
