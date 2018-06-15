#ifndef INODE_H
#define INODE_H
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include "Disk.h"
using std::string;

/*
0 for super block, data bit map, Inode bit map
1~5 Inode table
6~645 data block
//0~9 superblock(0~1基本块大小(1024B)，2~9文件名), 10~89 Inode bit map 90~729 data bit map

*/
//0 isDir 1~3 fileSize 4~10 itime 11~17 ftime 18~20 competency 21~36 dataAdd 37 fileBlkSize 38~45 name 46 childNum
class Inode
{
private:

	//cost 3B
	int fileSize;	//range from 0 to 2^16, unit is bit
	//cost 7B = year(2) +month(1) +day(1) +hour(1) +minute(1) +second(1)

	int itime[7];	//time inode created
	//cost 7B
	int ftime[7];	//the last time file changed
	//cost 3B
	int competency[3];	//0 for read, 1 for write, 2 for execute
						//the lower value, the easier to execute
	//cost 16B = 8 * 2B
	int dataAdd[8];		//the address of data block
						//如果是文件夹，则在datablock中存子文件的inode编号，每个1B，因此文件夹最多占用1block
	int fileBlkSize;	//number of data block

	int childNum;	//如果是文件夹，有几个子文件
	Inode* child[80];

public:

	const static int blkSize = 1024;
	const static int maxDataBlk = 8;
	const static int dataBlkNum = 640;
	const static int maxinode = 80;

    //不用存
	int inodeNo;	//physical address;*/
    //8B
	string name;
	//cost 1B
	int isDir;		//0 for file, 1 for dir
	Inode* parent;

	Inode(int inodeNo, int isDir, string name, int *dBitMap, int *iBitMap);
	~Inode() {}
	void get_time(int* myTime); //获取系统时间，写到myTime中
	void show();
	Inode* find_child(string fileName);	//0 means a file, 1 for a dir	如果是文件夹，通过文件名找到子文件Inode
	void show_data(Disk& disk);
	void write_to_mem(Disk& disk);
	void read_from_mem(Disk& disk);
	void write_string(string& content);
	void delete_content(int *dBitMap);
	void write_content(Disk &disk, string& content, int *dBitMap);
	void add_content(Disk &disk, string& content, int *dBitMap);
	Inode* create_inode(string fileName, Disk &disk, int &inodeCnt);
	void new_child(Inode *newInode, Disk &disk);
	void show_children();
	void delete_child(string fileName, int *iBitMap);

	void init(int &dataBlkNum, int &inodeCnt, Disk &disk, int *dBitMap, int *iBitMap);	//将所有child从disk中取出
};

#endif
