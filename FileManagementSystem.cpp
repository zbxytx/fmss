#include "FileManagementSystem.h"

//0~9 superblock(0~1基本块大小(1024B)，2~9文件名), 10~89 Inode bit map 90~729 data bit map

int FileMngSys::iBitMap[maxInode+5];
int FileMngSys::dBitMap[dataBlkNum+5];

FileMngSys::FileMngSys() : root(1, 1, "Root", dBitMap, iBitMap)
{

    fstream tmp;
    tmp.open(disk.diskFile.c_str(), ios::in);
    if (!tmp)
    {
        disk.initDisk();
        for (int i=0; i<=maxInode; ++i)
            dBitMap[i] = 0;
        for (int i=1; i<=maxInode; ++i)
            iBitMap[i] = 0;

        Inode tmp(1, 1, "Root", dBitMap, iBitMap);
        root = tmp;
        root.write_to_mem(disk);
        inodeCnt = 1;
        dataBlkCnt = 0;
        root.init(dataBlkCnt, inodeCnt, disk, dBitMap, iBitMap);	//go through all its children, initialize those Inode
        workDir = &root;
    }
    else
    {
        char* buf = new char[disk.blkSize];
        disk.getBlk(buf, 0);

        for (int i=0; i<=maxInode; ++i)
            dBitMap[i] = buf[i+10];
        for (int i=0; i<=maxInode; ++i)
            iBitMap[i] = buf[i+90];

        root.read_from_mem(disk);
        inodeCnt = 1;
        dataBlkCnt = 0;
        root.init(dataBlkCnt, inodeCnt, disk, dBitMap, iBitMap);	//go through all its children, initialize those Inode
        workDir = &root;
    }
    tmp.close();
}

FileMngSys::~FileMngSys()
{
    char* buf = new char[disk.blkSize];

    disk.getBlk(buf, 0);
    for (int i=0; i<=maxInode; ++i)
        buf[i+10] = dBitMap[i];
    for (int i=0; i<=maxInode; ++i)
        buf[i+90] = iBitMap[i];

    disk.putBlk(buf, 0);
}


void FileMngSys::show_help()
{
	cout<<"Type in \"read\"+\"file name\" to read an exist file.\n";
	cout<<"Type in \"write\"+\"file name\" to write over a file, and if the file doesn't exist, it will create one.\n";
	cout<<"Type in \"add\"+\"file name\" to add content to a file, and if the file doesn't exist, it will create one.\n";
	cout<<"Remember to type in \"finished\" ";
	cout<<"Type in \"create\"+\"file name\" to create a new directory.\n";
	cout<<"Type in \"delete\"+\"file name\" to delete a file or directory.\n";
	cout<<"with a single line to told SFMS you finish writing.\n";
	cout<<"Type in \"ls\" to show current directory\n";
	cout<<"Type in \"cd..\" to get to parent directory.\n";
	cout<<"Type in \"cd\"+\"file name\" to go to that directory.\n";
	cout<<"Type in \"clean\" or \"clear\" to clean the screen.\n";
	cout<<"Type in \"quit\" or \"exit\" to leave SFMS\n";
}

int FileMngSys::read_command()
{
	string command;
	cin>>command;
	//command	//change it to lowercase
	if (command=="quit" || command=="exit")	return 0;
	if (command=="help")	return 1;
	if (command=="read")	return 2;
	if (command=="write")	return 3;
	if (command=="cd..")	return 4;
	if (command=="cd")		return 5;
	if (command=="clean" || command=="clear")	return 6;
	if (command=="ls") return 7;
	if (command=="add") return 8;
	if (command=="create") return 9;
	if (command=="delete") return 10;
	return -1;
}

void FileMngSys::show_current_dir()
{
	Inode* tmp = workDir;

	string path(tmp->name);
	while (tmp->parent!=NULL)
	{
		tmp = tmp->parent;
		path = tmp->name + "/" +path;
	}
	cout<<path;
}

void FileMngSys::quit()
{
	// need to do what?
}

void FileMngSys::read_file()
{
	Inode *tmp = NULL;
	string fileName;

	cin>>fileName;
	tmp = workDir->find_child(fileName);
	if (tmp==NULL)	cout<<errMsg5<<endl;
	else	tmp->show_data(disk);
}

void FileMngSys::write_file()	//may be we can add a flag to class node, that show if a file is a directory
{
	string fileName, content("");
	char* buf = new char[blkSize];
	Inode* tmp = NULL;
	//int blockNum;

	while (1)
	{
		cin>>fileName;
		if (fileName.length()<=8)	break;
		cout<<errMsg3<<endl<<sgsMsg2<<endl;
	}

	tmp = workDir->find_child(fileName);

	if (tmp == NULL)    //不存在这个文件
	{
	    tmp = create_inode(fileName, 0);
		if (tmp==NULL)	return;	//means fail to create a file
	}
	else if (tmp->isDir==1)
	{
		tmp->delete_content(dBitMap);
		tmp->isDir = 0;
	}
	else	tmp->delete_content(dBitMap);
	//till now we get a Inode with null content


	while (1)
	{
		gets(buf);
		if (strcmp(buf, "quit")==0 || strcmp(buf, "exit")==0 || strcmp(buf, "finished")==0)	break;
		content =  content + buf + ' ';
	}
	//till now we get the whole message that the user want to type in

	tmp->write_content(disk, content, dBitMap);
}

void FileMngSys::go_up()
{
	if (workDir->parent!=NULL)	workDir = workDir->parent;
}

void FileMngSys::go_down()
{
    string fileName;
	cin>>fileName;
	if (fileName=="..")	go_up();
	else if (workDir->find_child(fileName) == NULL)
    {
        cout << "没有这个文件夹" << endl;
    }
	else if (workDir->find_child(fileName)->isDir == 0)
	{
	    cout << "那不是文件夹" << endl;
	}
	else workDir = workDir->find_child(fileName);
}

void FileMngSys::run()
{
	cout<<"Welcome to use the Simple File Management System.\n";
	cout<<"-------------------------------------------------------------------\n";
	show_help();
	cout<<"You can get this message any time by typing \"help\" into console.\n";
	cout<<"Commands doesn't work when in writing mode.\n";
	cout<<"-------------------------------------------------------------------\n";

	while (1)
	{
		show_current_dir();

		cout<<": >>";
		int flag = read_command();

		if (flag==0)	quit();			//may be we should add command "del","list"...
		else if (flag==1)	show_help();
		else if (flag==2)	read_file();
		else if (flag==3)	write_file();
		else if (flag==4)	go_up();
		else if (flag==5)	go_down();
		else if (flag==6)	system("CLS");
		else if (flag==7)   show_children();
		else if (flag==8)   add_file();
		else if (flag==9)   create_directory();
		else if (flag==10)  delete_file();
		else	cout<<errMsg4<<endl<<sgsMsg3<<endl;
	}
}

Inode* FileMngSys::create_inode(string fileName, int isDir)
{
	if (inodeCnt==maxInode)
	{
		cout<<errMsg2<<endl<<sgsMsg0<<endl;
		return NULL;
	}
	if (dataBlkCnt==dataBlkNum)	// it seems that we don't need this judgement?
	{
		cout<<errMsg2<<endl<<sgsMsg1<<endl;
		return NULL;
	}

    Inode *newInode = new Inode(++inodeCnt, isDir, fileName, dBitMap, iBitMap);
	workDir->new_child(newInode, disk);

	newInode->write_to_mem(disk);
	workDir->write_to_mem(disk);

	return newInode;
}

void FileMngSys::show_children()
{
    workDir->show_children();
}

void FileMngSys::add_file()	//may be we can add a flag to class node, that show if a file is a directory
{
	string fileName, content("");
	char* buf = new char[blkSize];
	Inode* tmp = NULL;
	//int blockNum;

	while (1)
	{
		cin>>fileName;
		if (fileName.length()<=8)	break;
		cout<<errMsg3<<endl<<sgsMsg2<<endl;
	}

	tmp = workDir->find_child(fileName);
	if (tmp == NULL)    //不存在这个文件
	{
	    tmp = create_inode(fileName, 0);
		if (tmp==NULL)	return;	//means fail to create a file
	}
	else if (tmp->isDir==1)
	{
		cout << "那是一个文件夹" << endl;
		return;
	}
	//till now we get a Inode with null content

	while (1)
	{
		gets(buf);
		if (strcmp(buf, "quit")==0 || strcmp(buf, "exit")==0 || strcmp(buf, "finished")==0)	break;
		content = content + buf + ' ';
	}
	//till now we get the whole message that the user want to type in

	tmp->add_content(disk, content, dBitMap);
}

void FileMngSys::create_directory()
{
    string fileName, content("");
	char* buf = new char[blkSize];
	Inode* tmp = NULL;
	//int blockNum;

	while (1)
	{
		cin>>fileName;
		if (fileName.length()<=8)	break;
		cout<<errMsg3<<endl<<sgsMsg2<<endl;
	}

	tmp = workDir->find_child(fileName);
	if (tmp == NULL)    //不存在这个文件
	{
	    tmp = create_inode(fileName, 1);
		if (tmp==NULL)	return;	//means fail to create a file
	}
	else if (tmp->isDir==1)
    {
        cout << "文件夹已存在" << endl;
    }
    else cout << "存在同名文件" << endl;
}

void FileMngSys::delete_file()
{
    string fileName;
    cin >> fileName;

    if (!workDir->find_child(fileName))
    {
        cout << "没有那个文件" << endl;
        return;
    }
    workDir->delete_child(fileName, iBitMap);
}

