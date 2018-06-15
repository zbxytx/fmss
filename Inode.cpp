#include "Inode.h"

Inode::Inode(int inodeNo, int isDir, string name, int *dBitMap, int *iBitMap)
{
    this->inodeNo = inodeNo;
    this->isDir = isDir;
    this->name = name;
    parent = NULL;
    fileSize = 0;
    get_time(itime);
    get_time(ftime);
    memset(competency, 0, sizeof(competency));
    for (int i=0; i<8; i++)
        dataAdd[i] = -1;
    fileBlkSize = 0;
    childNum = 0;
    for (int i=0; i<80; i++)
        child[i] = NULL;

    iBitMap[inodeNo] = 1;

    if (isDir == 1)
    {
        for (int i=0; i<dataBlkNum; i++)
        {
            if (dBitMap[i] == 0)
            {
                dBitMap[i] = 1;
                dataAdd[0] = i;
                break;
            }
        }
    }
}

void Inode::init(int &dataBlkCnt, int &inodeCnt, Disk &disk, int *dBitMap, int *iBitMap)
{
    dataBlkCnt += fileBlkSize;

    if (isDir == 0) return;

    char *buf = new char[disk.blkSize];
    disk.getBlk(buf, 6+dataAdd[0]);

    for (int i=0; i<childNum; i++)
    {
        child[i] = new Inode(buf[i], 0, "tmp", dBitMap, iBitMap);
    }


    delete[] buf;

	for (int i=0; i<childNum; i++)
	{
		child[i]->read_from_mem(disk);
		inodeCnt ++;
		child[i]->init(dataBlkCnt, inodeCnt, disk, dBitMap, iBitMap);
	}

	delete[] buf;
}

void Inode::get_time(int* myTime)
{
	std::time_t t = std::time(0);
	struct tm *now = std::localtime(&t);

	myTime[0] = now->tm_sec;
	myTime[1] = now->tm_min;
	myTime[2] = now->tm_hour;
	myTime[3] = now->tm_mday;
	myTime[4] = now->tm_mon+1;
	myTime[5] = (now->tm_year+1900)%256;
	myTime[6] = (now->tm_year+1900)>>8;
}

Inode* Inode::find_child(string fileName)
{
	for (int i=0; i<childNum; i++)
	{
		if (fileName == child[i]->name)
		{
			return child[i];
		}
	}

	return NULL;
}

void Inode::show_data(Disk& disk)
{
	int cnt=0;
	char* buf = new char[disk.blkSize];

	for (int i=0; i<fileBlkSize; ++i)
	{
		disk.getBlk(buf, 6+dataAdd[i]);
		for (int j=0; cnt<fileSize; ++j, ++cnt)
			cout<<buf[j];
	}
	cout<<endl;
}

void Inode::read_from_mem(Disk& disk)	//an Inode with inodeNo get other messages from the memory
{
	int tableNo = (inodeNo-1)/16;
	int offset = (inodeNo-1)%16*64;
	char* buf = new char[disk.blkSize];
	disk.getBlk(buf, tableNo+1);

	//cout << tableNo << endl << offset << endl;

	isDir = buf[offset];
	fileSize = buf[offset+1]+(buf[offset+2]<<8)+(buf[offset+3]<<16);
	for (int i=0; i<7; ++i)
		itime[i] = buf[offset+4+i];
	for (int i=0; i<7; ++i)
		ftime[i] = buf[offset+11+i];
	for (int i=0; i<3; ++i)
		competency[i] = buf[offset+18+i];
	for (int i=0; i<8; ++i)
    {
		dataAdd[i] = buf[offset+21+2*i]*100+buf[offset+22+2*i];
    }
	fileBlkSize = buf[offset+37];
	for (int i=0; i<8; i++)
		name[i] = buf[offset+38+i];
    childNum = buf[offset+46];
	//the assignment to var fileBlkSize,
	//how should we use blkSize, from object disk or static var in Disk

	delete[] buf;
}

void Inode::write_to_mem(Disk& disk)
{
	int tableNo = (inodeNo-1)/16;
	int offset = (inodeNo-1)%16*64;
	char* buf = new char[disk.blkSize];
	disk.getBlk(buf, tableNo+1);

	//cout << tableNo << endl << offset << endl;

	buf[offset] = isDir%256;
	buf[offset+1] = fileSize%256;
	buf[offset+2] = (fileSize>>8)%256;
	buf[offset+3] = (fileSize>>16)%256;
	for (int i=0; i<7; ++i)
		buf[offset+4+i] = itime[i];
	for (int i=0; i<7; ++i)
		buf[offset+11+i] = ftime[i];
	for (int i=0; i<3; ++i)
		buf[offset+18+i] = competency[i];
	for (int i=0; i<8; ++i)
	{
		buf[offset+21 +2*i] = dataAdd[i]/100;
		buf[offset+22 +2*i] = dataAdd[i]%100;
	}
	buf[offset+37] = fileBlkSize;
	for (int i=0; i<name.length(); i++)
    {
            buf[offset+38+i] = name[i];
    }
    buf[offset+46] = childNum;

	disk.putBlk(buf, tableNo+1);

	delete[] buf;
}

void Inode::delete_content(int *dBitMap)
{
	for (int i=0; i<fileBlkSize; ++i)
	{
		dBitMap[dataAdd[i]] = 0;
		dataAdd[i] = -1;
	}
	fileBlkSize = 0;
	fileSize = 0;
}

void Inode::new_child(Inode *newInode, Disk &disk)
{
    newInode->parent = this;
    child[childNum] = newInode;

    char *buf = new char[blkSize];
    disk.getBlk(buf, 6+dataAdd[0]);
    buf[childNum] = newInode->inodeNo;
    disk.putBlk(buf, 6+dataAdd[0]);
    childNum ++;

    /*int tableNo = (inodeNo-1)/16;
	int offset = (inodeNo-1)%16*64;
	char* buf = new char[blkSize];

	disk.getBlk(buf, tableNo+1);
	buf[offset

	delete[] buf;*/

}

void Inode::write_content(Disk &disk, string& content, int *dBitMap)
{
	char* buf = new char[blkSize];

	int len = content.length();
	if (len+fileSize > blkSize*maxDataBlk)
	{
		cout<<errMsg6<<endl;
		content.resize(blkSize*maxDataBlk);
	}

	for (int i=0; i*blkSize<len; i++)
    {
        for (int j=0; j<blkSize && j+i*blkSize<len; j++)
        {
            buf[j] = content[j+i*blkSize];
            fileSize ++;
        }
        if (dataAdd[i] == -1)
        {
            for (int k=0; k<dataBlkNum; k++)
            {
                if (dBitMap[k] == 0)
                {
                    //cout << k << endl;
                    dBitMap[k] = 1;
                    dataAdd[i] = k;
                    break;
                }
            }
        }

        disk.putBlk(buf, 6+dataAdd[i]);
        fileBlkSize ++;
        if (dBitMap[dataAdd[i]] == 0) dBitMap[dataAdd[i]] = 1;
    }

    write_to_mem(disk);

    delete[] buf;
}

void Inode::add_content(Disk &disk, string& content, int *dBitMap)
{
    if (fileBlkSize == 0)
        write_content(disk, content, dBitMap);
    else
    {
        char* buf = new char[blkSize+5];

        int len = content.length();
        if (len+fileSize > blkSize*maxDataBlk)
        {
            cout<<errMsg6<<endl;
            content.resize(blkSize*maxDataBlk);
        }

        disk.getBlk(buf, 6+dataAdd[fileBlkSize-1]);
        //cout << strlen(buf) << endl;
        string temp(buf);
        content = temp+ ' ' +content;
        len = content.length();

        delete[] buf;
        char* new_buf = new char[blkSize+5];

        fileSize -= fileSize%blkSize;
        fileBlkSize --;
        for (int i=fileBlkSize; i*blkSize<len; i++)
        {
            for (int j=0; j<blkSize && j+i*blkSize<len; j++)
            {
                new_buf[j] = content[j+i*blkSize];
                fileSize ++;
            }
            if (dataAdd[i] == -1)
            {
                for (int k=0; k<dataBlkNum; k++)
                {
                    if (dBitMap[k] == 0)
                    {
                        dBitMap[k] = 1;
                        dataAdd[i] = k;
                        break;
                    }
                }
            }

            disk.putBlk(new_buf,  6+dataAdd[i]);
            fileBlkSize ++;
        }

        write_to_mem(disk);

        delete[] new_buf;
    }
}

void Inode::show_children()
{
    for (int i=0; i<childNum; i++)
    {
        cout << child[i]->name;
        if (child[i]->isDir) cout <<"£¨ÎÄ¼þ¼Ð£©";
        cout << ' ';
    }
    cout << endl;
}

void Inode::delete_child(string fileName, int *iBitMap)
{
    if (isDir == 0) return;

    int i;
    for (i=0; i<childNum; i++)
    {
        if (child[i]->name == fileName)
        {
            break;
        }
    }

    iBitMap[child[i]->inodeNo] = 0;
    for (int j=i; j<childNum; j++)
    {
        child[j] = child[j+1];
    }
    childNum --;
}
