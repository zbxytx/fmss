#include "Disk.h"

Disk::Disk()
{
    diskFile = "Mydisk";
	ifstream disk;
	char* buf;

	disk.open(diskFile.c_str(), ifstream::in);
	if (!disk.is_open())
	{
		cout<<errMsg0<<endl;
		/*if (!initDisk())
		{
			cout<<errMsg1<<endl;
			return;
		}*/
		return;
	}
	buf = new char[2];
	disk.read(buf, 2);
	blkSize = (int)(buf[0]*100+buf[1]);

	disk.close();
	delete[] buf;
}

Disk::~Disk()
{
}

bool Disk::initDisk()
{
	ofstream disk;
	disk.open(diskFile.c_str(), ofstream::out);
	if (!disk.is_open())	return false;
	disk.close();

	blkSize = 1024;

	char* buf = new char[blkSize];
	memset(buf, 0, sizeof(buf));
	putBlk(buf, 1024);

	buf[0] = 10;	//1024的百千位
	buf[1] = 24;	//1024的个十位
	buf[2] = 'M';
	buf[3] = 'y';
	buf[4] = 'd';
	buf[5] = 'i';
	buf[6] = 's';
	buf[7] = 'k';
	buf[8] = '\0';
	buf[10] = 1;
	putBlk(buf, 0);	//we don't care what's in 95~1023

	delete[] buf;
	return true;
}

int Disk::getBlk(char* buf, int blkNum)	//读第blkNum块
{
	ifstream disk;

	disk.open(diskFile.c_str());
	if (!disk.is_open())
	{
		cout<<errMsg0<<endl;
		return 0;
	}
	disk.seekg(disk.beg+blkNum*blkSize);
	disk.read(buf, blkSize);

	disk.close();
	return 1;
}

void Disk::putBlk(char* buf, int blkNum)
{
	ofstream disk;
	disk.open(diskFile.c_str(), ios::in);

	disk.seekp(disk.beg+blkNum*blkSize);
	disk.write(buf, blkSize);
	//disk.seekp(ios::end);

	disk.close();
}
