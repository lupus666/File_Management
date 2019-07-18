#ifndef STRUCTURE_H
#define STRUCTURE_H
#include<QString>
#include<QDateTime>
#include<QList>
#include<QMap>
#include<QMessageBox>
#include<QTextStream>
#include<QTextCodec>
#pragma execution_character_set("utf-8")

enum FileType{
    UNKNOWN, COMMON, DIRECTORY
};

const static char*FileType_array[]={
    "UNKNOWN", "COMMON", "DIRECTORY"
};

struct SuperBlock{  //超级块
    int inodeNUM;
    int freeinodeNUM;
    SuperBlock(int num = 2048, int free_num = 2048):inodeNUM(num), freeinodeNUM(free_num){}
};

struct Inode{   //索引结点
    FileType file_type;
    QString file_name;
    int file_size;
    QDateTime create_time;
    QDateTime update_time;
    int block_num;
    QList<int> DataBlockVec;
    int child_num;
    QList<int> ChlidInodeVec;
    int father_index;
    Inode(){
        file_name = "";
        file_type = FileType::UNKNOWN;
        block_num = 0;
        child_num = 0;
        father_index = -1;
        file_size = 0;
        create_time = QDateTime();
        update_time = QDateTime();
        DataBlockVec = QList<int>();
        ChlidInodeVec = QList<int>();
    }
};

struct DataBlock{   //数据块
    QString data;
    DataBlock(){
        data.resize(2048);
        for(int i = 0; i < 2048; i++){
            data[i] = ' ';
        }
    }
};

struct GroupDescriptor{    //组描述符
    bool* DataBlockBitArray;
    bool* InodeBitArray;
};

class BlockGroup
{
public:
    int disk_size;      //磁盘大小
    int block_num;      //数据块数量
    SuperBlock superblock;      //超级块
    GroupDescriptor groupdescriptor;     //组描述符
    QList<DataBlock*> DataBlockList;     //数据块列表
    QList<Inode*> InodeList;     //索引结点列表

    int current_inode_index;    //当前目录

    BlockGroup();  //默认构造函数
    BlockGroup(int size, int inodeNum);     //初始化块组

    bool writeFileToVirtualDisk(int inodeindex, QString content);  //写文件到虚拟磁盘
    QString readFileFromVirtualDisk(int inodeindex); //从虚拟磁盘读文件
    bool createFile(FileType filetype, QString filename);   //在当前目录创建文件
    void deleteFile(int inodeindex);  //在当前目录删除文件
    void SaveToRealDisk();  //保存到真实磁盘
    bool ReadFromRealDisk();    //从真实磁盘中读取

    int FindFirstFreeInode();   //找到第一个空闲目录结点
    int FindFirstFreeDataBlock();   //找到第一个空闲数据块
    int FindInodeByName(FileType filetype, QString filename, int fatherindex);  //根据文件名在父目录查找文件
    void updateInode(Inode *inode); //更新目录项信息,自底向上
    void InodeClear(Inode *inode);  //清除索引结点
    QString GetPath();
};


#endif // STRUCTURE_H
