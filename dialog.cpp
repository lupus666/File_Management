#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent, QString content, bool ro, int index) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    ui->textEdit->setText(content);
    ui->textEdit->setReadOnly(ro);
    ui->Confirm->setEnabled(!ro);
    now_index = index;
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_Quit_clicked()
{
    this->close();
}

void Dialog::on_Confirm_clicked()
{
    emit dialog_send_a_signal(ui->textEdit->toPlainText());
    this->close();
}
