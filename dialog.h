#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr, QString content = "", bool ro = true, int index = -1);
    ~Dialog();
    Ui::Dialog* getUI(){return ui;}
private slots:
    void on_Quit_clicked();

    void on_Confirm_clicked();

private:
    Ui::Dialog *ui;
    int now_index;

signals:
    void dialog_send_a_signal(QString content);
};

#endif // DIALOG_H
