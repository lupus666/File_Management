# File_Management
基于qt5界面库的文件管理模拟系统
## 项目需求
- 在内存中开辟一个空间作为文件存储器，在其上实现一个简单的文件系统
- 退出这个文件系统时，需要该文件系统的内容保存到磁盘上，以便下次可以将其恢复到内存中来
- 文件存储空间管理自选一种方法
- 空闲空间管理自选一种方法
- 文件目录采用多级目录结构，目录项目中应包含文件名、物理地址、长度等信息。
- 文件系统应提供如下操作：格式化、创建子目录、删除子目录、显示目录、更改当前目录、创建文件、打开文件、关闭文件、写文件、读文件、删除文件

## 开发/运行环境
- 开发环境：Windows 10 1809
- 开发语言：C++11
- 开发工具：QT Creator 4.9.0
- 开发框架：QT 5.12.2

## 设计过程
一定程度上借鉴了学长的设计经验，并做出一些简化和改进
### 系统架构
![系统架构](https://github.com/lupus666/File_Management/raw/master/image/结构图.png)
- 用户界面，即用户直接进行操作的图形窗口界面
- 文件操作，即用户可以进行的文件操作
- 数据操作接口，即实现上层文件操作的相关数据结构的操作接口
- 虚拟磁盘，即该文件系统开辟的内存空间，所有的数据操作都是基于该内存空间进行的
- 真实磁盘，关闭该文件系统时，创建一个真实文件，保存文件系统的数据到真实磁盘，以便下次打开可以恢复到内存中来

该文件系统实现了一定的封装性，即用户无法了解到文件操作层以下的实现，一方面简化了用户的操作，另一方面也保障了文件系统的稳定性。
### 用户界面
整个文件系统有3个窗口：
- 主窗口  
    分为两个部分，一部分展示了当前目录的信息（文件/目录列表），另一部分展示了用户可以进行的文件操作
- 输入窗口  
    用于接受用户的输入（磁盘大小，文件/目录名）
- 读文件/写文件窗口  
    用于给用户提供文件的读写操作  

详细的介绍在[界面使用说明](#界面使用说明)

### 数据结构：块组（BlockGroup）
为了简化模拟，这里的块组（卷）相当于一个虚拟磁盘
```C++
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
};
```
#### 卷大小
这里的卷大小只包含了存储数据块的大小，即不包含存储文件控制块的大小
#### 数据块数量
为了简化以及方便操作，用于记录该卷所分配到的数据块的数量
#### 超级块
用于描述该卷中文件控制块FCB（索引结点inode）的信息
- FCB的数量
- 空闲索引的数量

```C++
struct SuperBlock{  //超级块
    int inodeNUM;
    int freeinodeNUM;
    SuperBlock(int num = 2048, int free_num = 2048):inodeNUM(num), freeinodeNUM(free_num){}
};
```


#### 组描述符
用于记录索引结点和数据块的空闲空间列表，这里采用位图的方式进行记录
- 数据块块位图  
    描述该卷中数据块的使用情况，True代表数据块空闲，False代表数据块已被占用
- FCB位图
    描述该卷中FCB的使用情况，True代表FCB空闲，False代表FCB已被占用

```C++
struct GroupDescriptor{    //组描述符
    bool* DataBlockBitArray;
    bool* InodeBitArray;
};
```

#### 数据块列表
用于记录该卷所包含的所有数据块  
- 数据块
    一个数据块有一个固定长度的QString（相当于std::String），大小为4KB

```C++
struct DataBlock{   //数据块
    QString data;
    DataBlock(){
        data.resize(2048);
        for(int i = 0; i < 2048; i++){
            data[i] = ' ';
        }
    }
};
```

#### FCB表（索引结点表）
用于记录该卷所分配到的所有FCB项，FCB表的大小由超级块给出
- FCB项
    - 文件类型
    - 文件名
    - 文件大小
    - 创建时间
    - 更新时间
    - 所占数据块数量（文件类型为普通文件有效）
    - 所占数据块索引表（文件类型为普通文件有效）
    - 所包含的子文件FCB项数量（文件类型为目录文件有效）
    - 所包含的子文件的FCB项索引（文件类型为目录文件有效）
    - 父目录的FCB项的索引

```C++
enum FileType{
    UNKNOWN, COMMON, DIRECTORY
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
```

#### 当前FCB项索引
为了方便文件系统的管理实现，这里引入一个用于记录当前文件系统所处目录的FCB项的索引

### 关键文件操作实现细节
#### 文件系统的保存与读取
虚拟磁盘的关键信息按照一定的格式储存在真实磁盘的`disk.txt`文件中，因为格式相对固定且每次打开文件系统都会从读取`disk.txt`文件来初始化虚拟磁盘，所以一旦修改了`disk.txt`文件中的数据，有可能导致文件系统打开出错。除非能恢复到正确的格式，否则只能删除`disk.txt`文件来使得文件系统可重新启动
#### 文件/目录的创建与删除
##### 创建
文件/目录的创建时，文件系统通过在FCB位图中找到并分配一个空闲的FCB（索引结点）来记录这个文件的信息，如果没有找到空闲的FCB则会返回错误。文件创建并不分配数据块，只有在写入的时候才分配数据块。 
```C++
bool BlockGroup::createFile(FileType filetype, QString filename)    //创建文件/目录
{
    if(superblock.freeinodeNUM == 0){
        return false;
    }
    superblock.freeinodeNUM--;
    int inode_index = FindFirstFreeInode();
    if(inode_index == -1)
        return false;
    Inode *inode = InodeList[inode_index];

    //修改目录项
    inode->file_type = filetype;
    inode->file_name = filename;
    inode->create_time = QDateTime::currentDateTime();
    inode->update_time = inode->create_time;
    inode->father_index = current_inode_index;

    groupdescriptor.InodeBitArray[inode_index] = false;
    //修改父目录项
    if(current_inode_index == -1){  //判断是否为根目录
        return true;
    }
    Inode *father_inode = InodeList[current_inode_index];
    father_inode->ChlidInodeVec.append(inode_index);
    father_inode->child_num++;
    father_inode->update_time = QDateTime::currentDateTime();

    return true;
}
```
在修改这个分配到的FCB时，也要修改该目录的FCB的信息。  
##### 删除
文件/目录在删除时，需要分为两种不同的情况，删除文件和删除目录。  

删除文件时，只需将该文件占用的FCB和数据块释放，即在FCB位图和数据块位图对应位置改为True便可。因为每次分配新的FCB和数据块都会覆盖写入，因此在删除时不必清除其中的内容。  
  
删除目录时，如果该目录下存在子文件子目录，则需要递归地删除该目录下的文件，而且要注意避免重复删除修改父目录FCB
```C++
void BlockGroup::deleteFile(int inodeindex)     //删除文件/目录
{
    Inode *inode = InodeList[inodeindex];
    //修改父目录项
    int fatherindex = inode->father_index;
    if(fatherindex == current_inode_index){     //避免重复删除
        Inode *fatherinode = InodeList[fatherindex];
        fatherinode->ChlidInodeVec.removeOne(inodeindex);
        fatherinode->child_num--;
        updateInode(fatherinode);
    }
    //删除该文件/目录
    if(inode->file_type == FileType::COMMON){   //普通文件直接清除数据块
        for(int i = 0; i < inode->block_num; i++){
            groupdescriptor.DataBlockBitArray[inode->DataBlockVec[i]] = true;
        }
    }
    else{
        if(inode->child_num != 0){      //目录文件需要递归删除子目录
            for(int i = 0; i < inode->child_num; i++){
                deleteFile(inode->ChlidInodeVec[i]);
            }
        }
    }
    InodeClear(inode);
    groupdescriptor.InodeBitArray[inodeindex] = true;
    superblock.freeinodeNUM++;
}
```
该删除算法按照深度优先遍历，自底向上地删除该目录下的所有子文件，最后再删除自身
#### 当前目录的更改
该文件系统只提供单步目录的更改，即当前目录只能更改为其他有父/子关系的目录。当前目录的更改有两种情况，分别是进入下级目录和返回上级目录。  
因为我在块组中记录了当前目录FCB的索引，所以更改目录只需更改当前目录FCB索引的值便可以实现当前目录的更改，而索引的值可以通过当前目录FCB中存储的子文件FCB索引和父目录FCB索引来获得。
#### 文件的读取与写入
文件的读取相对简单，只需根据该文件FCB中的数据块索引，获取对应的数据块，并将数据连接起来便可  
```C++
QString BlockGroup::readFileFromVirtualDisk(int inodeindex)     //从虚拟磁盘读文件
{
    Inode *inode = InodeList[inodeindex];
    QString content = "";
    for(int i = 0; i < inode->block_num; i++){
        DataBlock* datablock = DataBlockList[inode->DataBlockVec[i]];
        QString temp = datablock->data;
        content = content + temp;
    }
    return content;
}
```
文件的写入相对复杂。
- 先遍历该文件已经占有的数据块，并覆盖写入数据
- 在遍历完成所占有数据块后，存在两种情况，数据块不足和数据块多余
    - 对于数据块不足的情况，需要循环分配空闲的数据块直到所有数据写入完成
    - 对于数据块多余的情况，需要将多余的数据块释放掉，即将其位图置为True

```C++
bool BlockGroup::writeFileToVirtualDisk(int inodeindex, QString content)    //写文件到虚拟磁盘
{
    Inode *inode = InodeList[inodeindex];
    int in_point = 0;       //文件字符串写指针
    int blockcount = 0;

    bool islack = true;     //数据块缺少标记
    for(; blockcount < inode->block_num; blockcount++){                     //将现有的数据块重写入
        DataBlock *datablock = DataBlockList[inode->DataBlockVec[blockcount]];
        for(int j = 0;j < 2048; j++){
            if(in_point < content.length()){
                datablock->data[j] = content[in_point];
                in_point++;
            }else{
                datablock->data[j] = ' ';
                islack = false;
            }
        }

    }

    if(islack == true){     //数据块不足，需要开辟更多空间
        while(in_point < content.length()){
            int datablockindex = FindFirstFreeDataBlock();
            if(datablockindex == -1){//空闲数据块不足
                updateInode(inode);
                return false;
            }
            DataBlock *datablock = DataBlockList[datablockindex];
            for(int j = 0;j < 2048; j++){
                if(in_point < content.length()){
                    datablock->data[j] = content[in_point];
                    in_point++;
                }else{
                    datablock->data[j] = ' ';
                }
            }
            inode->DataBlockVec.append(datablockindex);
            inode->block_num++;
            groupdescriptor.DataBlockBitArray[datablockindex] = false;
        }
    }else{      //数据块充足，判断是否存在多余数据块进行释放
        int excessNum = inode->block_num - blockcount - 1;
        for(int i = 0 ;i < excessNum; i++){
            groupdescriptor.DataBlockBitArray[inode->DataBlockVec.takeLast()] = true;
            inode->block_num--;
        }
    }

    updateInode(inode);
    return true;
}
```
其他实现过程可以直接参考源文件，所有代码都有必要的注释。
## 界面使用说明
### 主窗口
![1](https://github.com/lupus666/File_Management/raw/master/image/1.png)
当文件系统无法找到`disk.txt`文件时，会提示用户磁盘未格式化并且这时用户只能关闭系统或者格式化磁盘。  
主窗口左边的框架用于显示当前目录情况，其中文件大小的单位是`Byte`
### 输入磁盘大小窗口
![2](https://github.com/lupus666/File_Management/raw/master/image/2.png)
输入内容限制为大于0小于100的数字，否则会报错
### 创建文件/创建目录
![3](https://github.com/lupus666/File_Management/raw/master/image/3.png)
输入内容不得出现`\`字符，否则会报错
### 更改当前目录
![4](https://github.com/lupus666/File_Management/raw/master/image/4.png)
![5](https://github.com/lupus666/File_Management/raw/master/image/5.png)
进入下级目录需要选中相应的目录文件，而返回上级目录不需要选中
### 读写文件
![6](https://github.com/lupus666/File_Management/raw/master/image/6.png)
读文件时，只能读不能修改
![7](https://github.com/lupus666/File_Management/raw/master/image/7.png)
写文件是，可以进行写入，按确定按钮便可以进行写入

### 删除文件
![8](https://github.com/lupus666/File_Management/raw/master/image/8.png)
选中要删除的文件进行删除便可

### 保存到真实磁盘
![9](https://github.com/lupus666/File_Management/raw/master/image/9.png)
点击保存便可直接将当前文件系统的关键信息保存到真实磁盘  

## 存在问题
### 读写文件时的问题
在处理数据块内部碎片时，我使用了`' '`进行填充，也因为这个空格符，在下一次读取文件时会读取到若干个空格，写文件的时候就在这些空格后面写入，而无法接续上一次的末尾，而且会存在浪费空间的现象。  
另外，文件无法记录写入文件时的字体和颜色，在下一次的读取文件时会自动转化为系统默认的字体和颜色。
### 文件大小的问题
因为该文件系统最小的空间单位为数据块，因此文件大小是由文件占用数据块的数量决定的，即数据块*4KB，而无法精确到文件实际大小。
