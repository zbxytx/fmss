#ifndef DISK_H
#define DISK_H

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
//#include "Inode.h"
/*using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;*/
using namespace std;

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
const string sgsMsg3("You may type in \"help\" to learn how to use the SFMS.");


class Disk
{
public:
	Disk();
	~Disk();

    bool initDisk();
	int blkSize;
	int inodeCnt;
	string diskFile;
	int getBlk(char* buf, int blkNum);	//blkNum means the physical block address
	void putBlk(char* buf, int blkNum);
};

#endif
