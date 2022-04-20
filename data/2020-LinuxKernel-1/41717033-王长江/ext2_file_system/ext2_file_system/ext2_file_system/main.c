#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#define BLKSIZE 1024
typedef struct ext2_group_desc GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;
#define CMD_LENGTH 128
#define NUM 20
char dir_inode[20][20] = {0}; //将文件名和对应的inode号以":"连接后保存
int k = 0;//dir_inode当前索引位置
SUPER *sp;
GD *gp;
INODE *ip;
INODE *sub_ip;
DIR *dp;
char buf[BLKSIZE],temp[256];
int fd;
int firstdata,blksize,inodesize,iblock,sub_inode,old_ino_blk=0;//old_ino_blk上一目录的block号
int old_inode_block[10];
int j = 0;
char *dev = "/dev/sdb2"; // default device name
int inode_start[3] = {5,8197,16389};//用sudo fsstat /dev/sdb2查看group信息，得到的每个group中Inode Table的起始位置
int get_block(int fd, int blk, char *buf){
	lseek(fd, blk*BLKSIZE, 0);
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
	blksize = 1024*(1<<sp->s_log_block_size);
	inodesize = sp->s_inode_size;
	printf("first_data_block=%d block_size=%d inode_size=%d\n",firstdata, blksize, inodesize);
	get_block(fd, (firstdata+1), buf); // get group descriptor
	gp = (GD *)buf;
	iblock = gp->bg_inode_table;
	/*
	printf("root inode information:\n");
	printf("---------------------–\n");
	printf("iblock=%d\n",iblock);
	*/
	get_block(fd, iblock, buf);
	ip = (INODE *)buf + 1; // ip point at #2 INODE
	/*
	printf("mode=%4x ", ip->i_mode);
	printf("uid=%d gid=%d\n", ip->i_uid, ip->i_gid);
	printf("size=%d\n", ip->i_size);
	printf("links=%d\n", ip->i_links_count);
	*/
}
int dir(int i_block){
	//遍历DIR数据块中条目
	char *cp;
	get_block(fd, i_block, buf); // get data block into buf[] 
	dp = (DIR *)buf;

	printf("inode\trec_len\tname_len\tname\n");
	cp = buf;
	int i = 0;
	char sbuf[20];
	while(cp < buf + BLKSIZE){
		strncpy(temp, dp->name, dp->name_len); // make name a string
		temp[dp->name_len] = 0; // in temp[ ]
		printf("%d\t%d\t%d\t\t%s\n", dp->inode, dp->rec_len, dp->name_len, temp);
		sprintf(sbuf,"%s%d",strcat(temp,":"),dp->inode);
		strncpy(dir_inode[k++], sbuf, strlen(sbuf));
		cp += dp->rec_len; // advance cp by rec_len
		dp = (DIR *)cp; // pull dp to next entry
	}
}
int search(char *name){//根据文件名遍历dir_inode，得到对应的inode号
	int i;
	for (i=0; i<k; i++){
		//printf("%s\n",dir_inode[i]);
		char *p = strtok(dir_inode[i], ":");
		if (strcmp(name, p)==0){
			p = strtok(NULL, " ");
			//old_inode = atoi(p);
			//printf("%s\n",p);
			//printf("found %s : inumber = %d\n", name, atoi(p));
			return atoi(p);
		}
	}
	return 0;
}
INODE* get_inode_point(int inode){//根据得到的inode号计算偏移量，获取对应的INODE *
	int ino = (sub_inode - 1) % 1680;
	int group = (sub_inode - 1) / 1680;
	printf("ino=%d group=%d\n",ino,group);
	int offset = inode_start[group] * BLKSIZE + ((sub_inode - 1) % 1680) * inodesize;
	lseek(fd, offset, 0);
	read(fd, buf, inodesize);
	return (INODE *) buf;
}
void show_old_inode_block(){
	printf("j=%d\n",j);
	for(int i=0;i<j;i++){
		printf("[%d]:%d\t",i,old_inode_block[i]);
	}
	printf("\n");
}
int main(int argc, char *argv[ ]){
	if (argc>1) dev = argv[1];
			inode(dev);
	while(1){
		printf("please input command(dir/quit):");
		//2、获取用户输入命令
		char cmd[CMD_LENGTH] = { 0 };
		fgets(cmd, 127, stdin);
		cmd[strlen(cmd) - 1] = 0;//去掉最后的回车

		if (strlen(cmd) == 0){//未输入命令,等待用户输入命令
			continue;
		}
    
		//3、解析命令,提取命令和参数
		char *cmd_arr[NUM] = { 0 };//存储命令各个部分,[0]:dir/quit [1]、[2]、、、:各命令参数
		char *p = strtok(cmd, " ");//以空格分割命令cmd
		int index = 0;
		while (p != NULL && index < NUM){
			cmd_arr[index++] = p;
			p = strtok(NULL, " ");
		}

		//4、执行命令
		if (strlen(cmd_arr[0]) == 3 && strncmp(cmd_arr[0], "dir", 3) == 0){//显示目录
			//inode(dev);
			if (old_ino_blk != 0){
				old_inode_block[j++] = old_ino_blk;
			}
			if (cmd_arr[1] == 0){
				printf("Show root directory\n");
				for (int i=0; i<12; i++){
					if (ip->i_block[i]){ 
						printf("i_block[%d]=%d\n", i, ip->i_block[i]);
						old_ino_blk = ip->i_block[i];
						dir(ip->i_block[i]);
					}
				}
			}else{
				sub_inode = search(cmd_arr[1]);
				if(sub_inode == 0){
					printf("This dir not exist!\n");
				} else {
				 	printf("Success\n");
					printf("Show sub directory:dir=%s inode=%d\n",cmd_arr[1],sub_inode);
					printf("inode_table=%d\n",iblock);
					sub_ip = get_inode_point(sub_inode);
				}
				
				for (int i=0; i<12; i++){ 
					if (sub_ip->i_block[i]){ 
						printf("i_block[%d]=%d\n", i, sub_ip->i_block[i]);
						old_ino_blk = sub_ip->i_block[i];
						dir(sub_ip->i_block[i]);;
						break;
					}
				}
			}
		} else if (strlen(cmd_arr[0]) == 4 && strncmp(cmd_arr[0], "back", 4) == 0){//退出
           	printf("Back\n");
           	dir(old_inode_block[--j]);
            /*sub_ip = get_inode_point(old_inode[j--]);
            for (int i=0; i<12; i++){ 
				if (sub_ip->i_block[i]){ 
					printf("i_block[%d]=%d\n", i, sub_ip->i_block[i]);
					dir(sub_ip->i_block[i]);
					break;
				}
			}*/
        } else if (strlen(cmd_arr[0]) == 4 && strncmp(cmd_arr[0], "quit", 4) == 0){//退出
           	printf("Quit\n");
            exit(0);
        } else {
            printf("command do not meet requirement,please enter again!\n");
        }
        //show_old_inode_block();
        printf("=========================================================\n");
	}
}
