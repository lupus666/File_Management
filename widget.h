#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "structure.h"
#include <QInputDialog>
#include "dialog.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    void update();  //更新文件显示

private slots:
    void on_Read_clicked();

    void on_Initialize_clicked();

    void on_SaveAndQuit_clicked();

    void on_Creat_File_clicked();

    void on_CreateDir_clicked();

    void on_EnterNextDir_clicked();

    void on_ReturnFather_clicked();

    void on_Write_clicked();

    void on_Rename_clicked();

    void on_Delete_clicked();

public slots:
    void mainwindows_receive_a_signal(QString content);

private:
    Ui::Widget *ui;
    BlockGroup *blockgroup;
    Dialog *dialog;
    int now_index;
};

#endif // WIDGET_H
