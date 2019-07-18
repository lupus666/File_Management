#include"structure.h"


BlockGroup::BlockGroup()
{
    disk_size = 0;
    block_num = 0;
    current_inode_index = 0;
}

BlockGroup::BlockGroup(int size, int inodeNum)  //格式化
{
    disk_size = size;
    int size_of_bytes = disk_size * 1024 * 1024;
    block_num = size_of_bytes/4096;             //虚拟磁盘数据块初始化
    //超级块初始化
    superblock.inodeNUM = inodeNum;
    superblock.freeinodeNUM = inodeNum;
    InodeList = QList<Inode*>();
    DataBlockList = QList<DataBlock*>();
    //组描述符初始化
    groupdescriptor.DataBlockBitArray = new bool[unsigned(block_num)];
    groupdescriptor.InodeBitArray = new bool[unsigned(inodeNum)];
    for(int i = 0; i < inodeNum; i++){  //目录项初始化
        groupdescriptor.InodeBitArray[i] = true;
        Inode *inode = new Inode();
        InodeList.append(inode);
    }
    for(int i = 0; i < block_num; i++){ //数据块初始化
        groupdescriptor.DataBlockBitArray[i] = true;
        DataBlock *datablock = new DataBlock();
        DataBlockList.append(datablock);
    }

    current_inode_index = -1;
    if(!createFile(FileType::DIRECTORY, "root")){
        //错误信息
    }
    current_inode_index = 0;
}

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

void BlockGroup::SaveToRealDisk()       //保存到真实磁盘中
{
    QFile file("disk.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Text);
    file.resize(0);
    //写入磁盘大小
    QString disksize_str = QString::number(disk_size) + '\n';
    file.write(disksize_str.toUtf8());
    //写入超级块
    QString superblock_str = QString::number(superblock.inodeNUM) + ' ';
    superblock_str = superblock_str + QString::number(superblock.freeinodeNUM) + '\n';
    file.write(superblock_str.toUtf8());
    //写入组描述符
    QString groupdescriptor_str = "";
    for(int i = 0; i < superblock.inodeNUM; i++){
        if(groupdescriptor.InodeBitArray[i] == false){
            groupdescriptor_str = groupdescriptor_str + QString::number(i) + ' ';
        }
    }
    groupdescriptor_str += "\n";
    for(int i = 0; i < block_num; i++){
        if(groupdescriptor.DataBlockBitArray[i] == false){
            groupdescriptor_str = groupdescriptor_str + QString::number(i) + ' ';
        }
    }
    groupdescriptor_str += "\n";
    file.write(groupdescriptor_str.toUtf8());
    //写入目录表
    for(int i = 0; i < superblock.inodeNUM; i++){
        if(groupdescriptor.InodeBitArray[i] == false){
            QString str ="";
            Inode *inode = InodeList[i];
            str = QString::number(inode->file_type) + ' ' + inode->file_name + ' '
                    + QString::number(inode->file_size) + ' ' + QString::number(inode->block_num) + ' '
                    + QString::number(inode->child_num) + ' ' + QString::number(inode->father_index) + ' '
                    + inode->create_time.toString("yyyy-MM-dd hh:mm:ss") + ' ' + inode->update_time.toString("yyyy-MM-dd hh:mm:ss") + ' ';
            for(int j = 0; j < inode->block_num; j++){
                str = str + QString::number(inode->DataBlockVec[j]) + '/';
            }
            for(int j = 0; j < inode->child_num; j++){
                str = str + QString::number(inode->ChlidInodeVec[j]) + '/';
            }
            str += '\n';
            file.write(str.toUtf8());
        }
    }
    //写入数据块
    for(int i = 0; i < block_num; i++){
        if(groupdescriptor.DataBlockBitArray[i] == false){
            DataBlock *datablock = DataBlockList[i];
            QString str = datablock->data;
            str += '\n';
            file.write(str.toUtf8());
        }
    }
    file.close();
}

bool BlockGroup::ReadFromRealDisk()     //从真实磁盘读取数据
{
    QFile file("disk.txt");
    if(file.open(QIODevice::ReadOnly | QIODevice::Text) == false){
        return false;//未格式化
    }
    else{
        //读入磁盘大小
        QByteArray disksize_str = file.readLine();
        disk_size = QString(disksize_str).toInt();
        block_num = disk_size *1024 *1024 /4096;
        //读入超级块
        QByteArray superblock_str = file.readLine();
        QList<QByteArray> superblock_str_list = superblock_str.split(' ');
        superblock.inodeNUM = superblock_str_list[0].toInt();
        superblock.freeinodeNUM = superblock_str_list[1].toInt();
        //读入组描述符
        groupdescriptor.DataBlockBitArray = new bool[unsigned(block_num)];
        groupdescriptor.InodeBitArray = new bool[unsigned(superblock.inodeNUM)];
        for(int i = 0; i < block_num; i++){
            groupdescriptor.DataBlockBitArray[i] = true;
        }
        for(int i = 0; i < superblock.inodeNUM; i++){
            groupdescriptor.InodeBitArray[i] = true;
        }
        QByteArray group_inode_str = file.readLine();
        QList<QByteArray> group_inode_list = group_inode_str.split(' ');
        for(int i = 0; i < group_inode_list.length() - 1; i++){
            groupdescriptor.InodeBitArray[group_inode_list[i].toInt()] = false;
        }
        QByteArray group_datablock_str = file.readLine();           //长度问题
        QList<QByteArray> group_datablock_list = group_datablock_str.split(' ');
        for(int i = 0; i < group_datablock_list.length() - 1; i++){
            groupdescriptor.DataBlockBitArray[group_datablock_list[i].toInt()] = false;
        }
        //读入目录表
        InodeList = QList<Inode*>();
        for(int i = 0; i < superblock.inodeNUM; i++){
            Inode *inode = new Inode();
            if(groupdescriptor.InodeBitArray[i] == false){
                QByteArray inode_ary = file.readLine();
                QString inode_str = QString(inode_ary);
                QList<QString> inode_list = inode_str.split(' ');
                inode->file_type = FileType(inode_list[0].toInt());
                inode->file_name = inode_list[1];
                inode->file_size = inode_list[2].toInt();
                inode->block_num = inode_list[3].toInt();
                inode->child_num = inode_list[4].toInt();
                inode->father_index = inode_list[5].toInt();
                inode->create_time = QDateTime::fromString(inode_list[6] + ' ' + inode_list[7], "yyyy-MM-dd hh:mm:ss");
                inode->update_time = QDateTime::fromString(inode_list[8] + ' ' + inode_list[9], "yyyy-MM-dd hh:mm:ss");
                QList<QString> dataOrChild_list = inode_list[10].split('/');
                for(int j = 0; j < inode->block_num ; j++){
                    inode->DataBlockVec.append(dataOrChild_list[j].toInt());
                }
                for(int j = 0; j < inode->child_num ; j++){
                    inode->ChlidInodeVec.append(dataOrChild_list[j].toInt());
                }

            }
            InodeList.append(inode);
        }
        //读入数据块
        DataBlockList = QList<DataBlock*>();
        for(int i = 0; i < block_num; i++){
            DataBlock *datablock = new DataBlock();
            if(groupdescriptor.DataBlockBitArray[i] == false){
                int count = 0;
                QString temp;
                while(count <= 2048){
                    QString data_str = file.readLine();
                    count += data_str.length();
                    temp += data_str;
                }
                datablock->data = temp;
            }
            DataBlockList.append(datablock);
        }
        current_inode_index = 0;
        file.close();
        return true;
    }
}

int BlockGroup::FindFirstFreeInode()    //找到第一个空闲FCB
{
    int inode_num = superblock.inodeNUM;
    for(int i = 0;i < inode_num; i++){
        if(groupdescriptor.InodeBitArray[i] == true)
            return i;
    }
    return -1;

}

int BlockGroup::FindFirstFreeDataBlock()    //找到第一个空闲数据块
{
    int datablock_num = DataBlockList.length();
    for(int i = 0;i < datablock_num; i++){
        if(groupdescriptor.DataBlockBitArray[i] == true)
            return i;
    }
    return -1;
}

int BlockGroup::FindInodeByName(FileType filetype, QString filename, int fatherindex)   //在当前结点按名搜索文件/目录
{
    if(filename == "root" && fatherindex == -1){
        return 0;
    }
    Inode *fatherinode = InodeList[fatherindex];
    for(int i = 0; i < fatherinode->child_num; i++){
        int childindex = fatherinode->ChlidInodeVec[i];
        if(InodeList[childindex]->file_name == filename && InodeList[childindex]->file_type == filetype){
            return childindex;
        }
    }
    return -1;
}

void BlockGroup::updateInode(Inode *inode)      //更新FCB数据
{
    //更新时间
    inode->update_time = QDateTime::currentDateTime();
    //更新文件大小
    if(inode->file_type == FileType::COMMON){
        inode->file_size = inode->block_num * 4096;
    }
    else if(inode->file_type == FileType::DIRECTORY){
        int temp_size = 0;
        for(int i = 0;i < inode->child_num; i++){
            temp_size += InodeList[inode->ChlidInodeVec[i]]->file_size;
        }
        inode->file_size = temp_size;
    }
    //向上递归更新父目录FCB数据
    if(inode->father_index != -1){
        updateInode(InodeList[inode->father_index]);
    }
}

void BlockGroup::InodeClear(Inode *inode)       //初始化FCB
{
    inode->file_size = 0;
    inode->file_type = FileType::UNKNOWN;
    inode->create_time = QDateTime();
    inode->update_time = inode->create_time;
    inode->block_num = 0;
    inode->file_name = "";
    inode->file_size = 0;
    inode->child_num = 0;
    inode->father_index = -1;
    inode->DataBlockVec.clear();
    inode->ChlidInodeVec.clear();
}

QString BlockGroup::GetPath()       //获取当前路径的字符串表示
{
    int fatherindex = InodeList[current_inode_index]->father_index;
    QString path = "\\" + InodeList[current_inode_index]->file_name;
    while(fatherindex != -1){
        path = "\\" + InodeList[fatherindex]->file_name + path;
        fatherindex = InodeList[fatherindex]->father_index;
    }
    return path;
}
