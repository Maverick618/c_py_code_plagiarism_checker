/*************** imap.c program **************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc GD;
#define BLKSIZE 1024
SUPER *sp;
GD *gp;
char buf[BLKSIZE];
int fd;
// get_block() reads a disk block into a buf[ ]
int get_block(int fd, int blk, char *buf){
	lseek(fd, (long)blk*BLKSIZE, 0);
	read(fd, buf, BLKSIZE);
}
int imap(char *device){
	int i, ninodes, blksize, imapblk;
	fd = open(device, O_RDONLY);
	if (fd < 0){
		printf("open %s failed\n", device); exit(1);
	}
	get_block(fd, 1, buf); // get superblock
	sp = (SUPER *)buf;
	ninodes = sp->s_inodes_count; // get inodes_count
	printf("ninodes = %d\n", ninodes);
	get_block(fd, 2, buf); // get group descriptor
	gp = (GD *)buf;
	imapblk = gp->bg_inode_bitmap; // get imap block number
	printf("imapblk = %d\n", imapblk);
	get_block(fd, imapblk, buf); // get imap block into buf[ ]
	for (i=0; i<=ninodes/8; i++){ // print each byte in HEX
		printf("%02x ", (unsigned char)buf[i]);
	}
	printf("\n");
}
char * dev="/dev/sdb2"; // default device
int main(int argc, char *argv[ ] ){
	if (argc>1) 
		dev = argv[1];
	imap(dev);
}
