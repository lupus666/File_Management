#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList()<<"文件类型"<<"文件名"<<"文件大小"<<"创建时间"<<"修改时间");
    blockgroup = new BlockGroup();
    if(!blockgroup->ReadFromRealDisk()){
        QMessageBox::warning(this, " ", "未找到磁盘映像");
        ui->path->setText("磁盘未格式化，请先进行磁盘格式化");
        ui->path->setStyleSheet("color:red;");
        ui->Creat_File->setEnabled(false);
        ui->Read->setEnabled(false);
        ui->Write->setEnabled(false);
        ui->Rename->setEnabled(false);
        ui->Delete->setEnabled(false);
        ui->CreateDir->setEnabled(false);
        ui->EnterNextDir->setEnabled(false);
        ui->ReturnFather->setEnabled(false);
        ui->SaveAndQuit->setEnabled(false);
    }else{
        update();
    }

}

Widget::~Widget()
{
    delete ui;
}

void Widget::update()       //更新当前目录显示
{
    ui->path->setStyleSheet("color:black;");
    QString path = blockgroup->GetPath();
    ui->path->setText(path);

    int current_index = blockgroup->current_inode_index;
    Inode *currentinode = blockgroup->InodeList[current_index];
    int child_num = currentinode->child_num;
    ui->tableWidget->setRowCount(child_num);
    for(int i = 0; i < child_num; i++){
        Inode *inode = blockgroup->InodeList[currentinode->ChlidInodeVec[i]];
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(FileType_array[inode->file_type]));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(inode->file_name));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(inode->file_size)));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(inode->create_time.toString("yyyy-MM-dd hh:mm:ss")));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(inode->update_time.toString("yyyy-MM-dd hh:mm:ss")));
    }
}

void Widget::on_Read_clicked()      //读文件操作
{
    QList<QTableWidgetItem*> item_list = ui->tableWidget->selectedItems();
    if(item_list.length() >= 2){
        QString filetype = item_list[0]->text();
        if(filetype == "COMMON"){
            QString filename = item_list[1]->text();
            int inodeindex = blockgroup->FindInodeByName(FileType::COMMON, filename, blockgroup->current_inode_index);
            Inode *inode = blockgroup->InodeList[inodeindex];
            QString content = "";
            for(int i = 0; i < inode->block_num; i++){
                content = content + QString(blockgroup->DataBlockList[inode->DataBlockVec[i]]->data);
            }
            content.trimmed();
            dialog = new Dialog(nullptr, content, true, inodeindex);    //创建读窗口
            dialog->setModal(false);
            dialog->show();
        }
    }

}

void Widget::on_Initialize_clicked()    //格式化操作
{
    bool ok;
    QString disk_size = QInputDialog::getText(this, tr("格式化磁盘"), tr("请输入磁盘大小（MB）"), QLineEdit::Normal, QString(), &ok);
    if(ok && !disk_size.isEmpty()){
        bool isNum;
        int diskSize = disk_size.toInt(&isNum);
        if(isNum){
            if( diskSize <= 0 || diskSize >100){
                QMessageBox::warning(this, " ", "请输入大于0MB小于100MB的磁盘大小");
            }
            else{
                blockgroup = new BlockGroup(diskSize, 2048);
                update();
                ui->Creat_File->setEnabled(true);
                ui->Read->setEnabled(true);
                ui->Write->setEnabled(true);
                ui->Rename->setEnabled(true);
                ui->Delete->setEnabled(true);
                ui->CreateDir->setEnabled(true);
                ui->EnterNextDir->setEnabled(true);
                ui->ReturnFather->setEnabled(true);
                ui->SaveAndQuit->setEnabled(true);
            }
        }
        else{
            QMessageBox::warning(this, " ", "请输入数字");
        }
    }
}

void Widget::on_SaveAndQuit_clicked()   //保存到磁盘操作
{
    if(blockgroup != nullptr){
        blockgroup->SaveToRealDisk();
    }
}

void Widget::on_Creat_File_clicked()    //创建文件操作
{
    bool ok;
    QString file_name = QInputDialog::getText(this, tr("创建文件"), tr("请输入文件名"), QLineEdit::Normal, QString(), &ok);
    if(ok && !file_name.isEmpty()){
        if(file_name.contains('\\')){
            QMessageBox::warning(this, " ", "文件名不得包含‘\\’");
            return;
        }
        if(blockgroup->FindInodeByName(FileType::COMMON, file_name, blockgroup->current_inode_index) != -1){
            QMessageBox::warning(this, " ", "已存在重名文件");
        }else{
            blockgroup->createFile(FileType::COMMON, file_name);
            update();
        }
    }
    else{
        if(file_name.isEmpty()){
            QMessageBox::warning(this, " ", "文件名不能为空");
        }
    }
}

void Widget::on_CreateDir_clicked()     //创建目录操作
{
    bool ok;
    QString file_name = QInputDialog::getText(this, tr("创建子目录"), tr("请输入目录名"), QLineEdit::Normal, QString(), &ok);
    if(ok && !file_name.isEmpty()){
        if(file_name.contains('\\')){
            QMessageBox::warning(this, " ", "目录名不得包含‘\\’");
            return;
        }
        if(blockgroup->FindInodeByName(FileType::DIRECTORY, file_name, blockgroup->current_inode_index) != -1){
            QMessageBox::warning(this, " ", "已存在重名目录");
        }else{
            blockgroup->createFile(FileType::DIRECTORY, file_name);
            update();
        }
    }
    else{
        if(file_name.isEmpty()){
            QMessageBox::warning(this, " ", "目录名不能为空");
        }
    }
}

void Widget::on_EnterNextDir_clicked()      //进入子目录操作
{
    QList<QTableWidgetItem*> item_list = ui->tableWidget->selectedItems();
    if(item_list.length()>=2){
        QString filetype = item_list[0]->text();
        if(filetype == "DIRECTORY"){
            QString filename =item_list[1]->text();
            ui->path->setText(filename);
            int inodeindex = blockgroup->FindInodeByName(FileType::DIRECTORY, filename, blockgroup->current_inode_index);
            if(inodeindex != -1){
                blockgroup->current_inode_index = inodeindex;
                update();
            }
        }
    }

}

void Widget::on_ReturnFather_clicked()      //返回父目录操作
{
    int fatherindex = blockgroup->InodeList[blockgroup->current_inode_index]->father_index;
    if(fatherindex != -1){
        blockgroup->current_inode_index = fatherindex;
        update();
    }
}

void Widget::on_Write_clicked()         //写文件操作
{
    QList<QTableWidgetItem*> item_list = ui->tableWidget->selectedItems();
    if(item_list.length() >= 2){
        QString filetype = item_list[0]->text();
        if(filetype == "COMMON"){
            QString filename = item_list[1]->text();
            int inodeindex = blockgroup->FindInodeByName(FileType::COMMON, filename, blockgroup->current_inode_index);
            now_index =inodeindex;
            Inode *inode = blockgroup->InodeList[inodeindex];
            QString content = "";
            for(int i = 0; i < inode->block_num; i++){
                content = content + QString(blockgroup->DataBlockList[inode->DataBlockVec[i]]->data);
            }
            content.trimmed();
            dialog = new Dialog(nullptr, content, false, inodeindex);
            dialog->setModal(false);
            dialog->show();
            connect(dialog, SIGNAL(dialog_send_a_signal(QString)), this, SLOT(mainwindows_receive_a_signal(QString)));
        }
    }

}

void Widget::mainwindows_receive_a_signal(QString content)  //接受来自写窗口的写入信号
{
    if(!blockgroup->writeFileToVirtualDisk(now_index, content)){
        QMessageBox::warning(this, " ", "虚拟磁盘空间不足");
    }
}

void Widget::on_Rename_clicked()    //重命名操作
{
    QList<QTableWidgetItem*> item_list = ui->tableWidget->selectedItems();
    if(item_list.length() >= 2){
        bool ok;
        QString newfilename = QInputDialog::getText(this, "重命名", "请输入新的文件/目录名", QLineEdit::Normal, QString(), &ok);
        QString filetype = item_list[0]->text();
        QString filename = item_list[1]->text();
        if(ok && !newfilename.isEmpty()){
            if(filetype == "COMMON"){
                if(blockgroup->FindInodeByName(FileType::COMMON, newfilename, blockgroup->current_inode_index) != -1){
                    QMessageBox::warning(this, " ", "已存在重名文件");
                }else{
                    int index = blockgroup->FindInodeByName(FileType::COMMON, filename, blockgroup->current_inode_index);
                    blockgroup->InodeList[index]->file_name = newfilename;
                    update();
                }
            }else if(filetype == "DIRECTORY"){
                if(blockgroup->FindInodeByName(FileType::DIRECTORY, newfilename, blockgroup->current_inode_index) != -1){
                    QMessageBox::warning(this, " ", "已存在重名目录");
                }else{
                    int index = blockgroup->FindInodeByName(FileType::DIRECTORY, filename, blockgroup->current_inode_index);
                    blockgroup->InodeList[index]->file_name = newfilename;
                    update();
                }
            }
        }
    }
}

void Widget::on_Delete_clicked()    //删除操作
{
    QList<QTableWidgetItem*> item_list = ui->tableWidget->selectedItems();
    if(item_list.length() >= 2){
        QString filetype = item_list[0]->text();
        QString filename = item_list[1]->text();
        if(filetype == "COMMON"){
            int index = blockgroup->FindInodeByName(FileType::COMMON, filename, blockgroup->current_inode_index);
            blockgroup->deleteFile(index);
            update();
        }
        else{
            int index = blockgroup->FindInodeByName(FileType::DIRECTORY, filename, blockgroup->current_inode_index);
            blockgroup->deleteFile(index);
            update();
        }
    }
}
