#ifndef FILEMANAGEMENTSYSTEM_H
#define FILEMANAGEMENTSYSTEM_H

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "Disk.h"
#include "Inode.h"
using namespace std;
/*using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::fstream;*/

const int maxInode = 80;	//total available Inode num
const int maxDataBlk = 8;	//max num of blk that a data blk can take
const int dataBlkNum = 640;	//total available data blk num
/*
const string errMsg0("Error, fail to find the disk file.");
const string errMsg1("Error, fail to create a disk file.");
const string errMsg2("Error, fail to create a new file.");
const string errMsg3("Error, you can't create a file with a name longer than 8 letters.");
const string errMsg4("Error, fail to recognize the command.");
const string errMsg5("Error, fail to find the file.");
const string errMsg6("Error, you can't create a file larger than 8096 Bytes.\nIt will ignore all contents subsequent.");

const string sgsMsg0("You can have at most 80 files(including directories).");
const string sgsMsg1("You may delete some file first.");
const string sgsMsg2("You may try a shorter name.");
const string sgsMsg3("You may type in \"help\" to learn how to use the SFMS.");*/

/*
0 for super block, data bit map, Inode bit map
1~5 Inode table
6~645 data block

*/

class FileMngSys
{
private:
	string diskName;
	Inode root;	//the 1st Inode
	Inode* workDir;	//working directory
	Disk disk;
	int inodeCnt, dataBlkCnt;

	void show_help();
	int read_command();
	void show_current_dir();
	void read_file();
	void write_file();
	void go_up();
	void go_down();
	void quit();
	Inode* create_inode(string fileName, int isDir);
	void show_children();
	void add_file();
	void create_directory();
	void delete_file();

	friend class Inode;
	const int blkSize = 1024;
public:
	static int iBitMap[maxInode+5];	//Does it work?
	static int dBitMap[dataBlkNum+5];

	FileMngSys();
	~FileMngSys();
	void run();
};

#endif
