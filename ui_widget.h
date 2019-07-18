/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QTableWidget *tableWidget;
    QSplitter *splitter;
    QPushButton *Creat_File;
    QPushButton *Read;
    QPushButton *Write;
    QPushButton *Rename;
    QPushButton *Delete;
    QSplitter *splitter_2;
    QPushButton *Initialize;
    QPushButton *CreateDir;
    QPushButton *EnterNextDir;
    QPushButton *ReturnFather;
    QPushButton *SaveAndQuit;
    QLabel *label;
    QLabel *path;
    QLabel *label_2;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(1109, 687);
        tableWidget = new QTableWidget(Widget);
        if (tableWidget->columnCount() < 5)
            tableWidget->setColumnCount(5);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        tableWidget->setGeometry(QRect(30, 100, 650, 450));
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setAlternatingRowColors(false);
        tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableWidget->setTextElideMode(Qt::ElideRight);
        tableWidget->setShowGrid(false);
        tableWidget->setGridStyle(Qt::NoPen);
        tableWidget->setWordWrap(true);
        tableWidget->setCornerButtonEnabled(true);
        tableWidget->setColumnCount(5);
        tableWidget->horizontalHeader()->setVisible(true);
        tableWidget->horizontalHeader()->setCascadingSectionResizes(false);
        tableWidget->horizontalHeader()->setDefaultSectionSize(130);
        tableWidget->horizontalHeader()->setHighlightSections(false);
        tableWidget->horizontalHeader()->setMinimumSectionSize(0);
        tableWidget->verticalHeader()->setVisible(false);
        splitter = new QSplitter(Widget);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setGeometry(QRect(910, 90, 180, 461));
        splitter->setOrientation(Qt::Vertical);
        splitter->setOpaqueResize(false);
        Creat_File = new QPushButton(splitter);
        Creat_File->setObjectName(QString::fromUtf8("Creat_File"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221 Light"));
        font.setPointSize(14);
        Creat_File->setFont(font);
        splitter->addWidget(Creat_File);
        Read = new QPushButton(splitter);
        Read->setObjectName(QString::fromUtf8("Read"));
        Read->setFont(font);
        splitter->addWidget(Read);
        Write = new QPushButton(splitter);
        Write->setObjectName(QString::fromUtf8("Write"));
        Write->setFont(font);
        splitter->addWidget(Write);
        Rename = new QPushButton(splitter);
        Rename->setObjectName(QString::fromUtf8("Rename"));
        Rename->setFont(font);
        splitter->addWidget(Rename);
        Delete = new QPushButton(splitter);
        Delete->setObjectName(QString::fromUtf8("Delete"));
        Delete->setFont(font);
        splitter->addWidget(Delete);
        splitter_2 = new QSplitter(Widget);
        splitter_2->setObjectName(QString::fromUtf8("splitter_2"));
        splitter_2->setGeometry(QRect(700, 90, 180, 461));
        splitter_2->setOrientation(Qt::Vertical);
        splitter_2->setOpaqueResize(false);
        Initialize = new QPushButton(splitter_2);
        Initialize->setObjectName(QString::fromUtf8("Initialize"));
        Initialize->setFont(font);
        splitter_2->addWidget(Initialize);
        CreateDir = new QPushButton(splitter_2);
        CreateDir->setObjectName(QString::fromUtf8("CreateDir"));
        CreateDir->setFont(font);
        splitter_2->addWidget(CreateDir);
        EnterNextDir = new QPushButton(splitter_2);
        EnterNextDir->setObjectName(QString::fromUtf8("EnterNextDir"));
        EnterNextDir->setFont(font);
        splitter_2->addWidget(EnterNextDir);
        ReturnFather = new QPushButton(splitter_2);
        ReturnFather->setObjectName(QString::fromUtf8("ReturnFather"));
        ReturnFather->setFont(font);
        splitter_2->addWidget(ReturnFather);
        SaveAndQuit = new QPushButton(splitter_2);
        SaveAndQuit->setObjectName(QString::fromUtf8("SaveAndQuit"));
        SaveAndQuit->setFont(font);
        splitter_2->addWidget(SaveAndQuit);
        label = new QLabel(Widget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(31, 21, 95, 25));
        label->setFont(font);
        path = new QLabel(Widget);
        path->setObjectName(QString::fromUtf8("path"));
        path->setGeometry(QRect(132, 21, 951, 61));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221 Light"));
        font1.setPointSize(11);
        path->setFont(font1);
        path->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        path->setWordWrap(true);
        label_2 = new QLabel(Widget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(930, 650, 171, 31));
        label_2->setFont(font1);

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "\346\226\207\344\273\266\347\263\273\347\273\237\346\250\241\346\213\237", nullptr));
        Creat_File->setText(QApplication::translate("Widget", "\345\210\233\345\273\272\346\226\207\344\273\266", nullptr));
        Read->setText(QApplication::translate("Widget", "\350\257\273\346\226\207\344\273\266", nullptr));
        Write->setText(QApplication::translate("Widget", "\345\206\231\346\226\207\344\273\266", nullptr));
        Rename->setText(QApplication::translate("Widget", "\351\207\215\345\221\275\345\220\215", nullptr));
        Delete->setText(QApplication::translate("Widget", "\345\210\240\351\231\244", nullptr));
        Initialize->setText(QApplication::translate("Widget", "\346\240\274\345\274\217\345\214\226", nullptr));
        CreateDir->setText(QApplication::translate("Widget", "\345\210\233\345\273\272\345\255\220\347\233\256\345\275\225", nullptr));
        EnterNextDir->setText(QApplication::translate("Widget", "\350\277\233\345\205\245\344\270\213\347\272\247\347\233\256\345\275\225", nullptr));
        ReturnFather->setText(QApplication::translate("Widget", "\350\277\224\345\233\236\344\270\212\347\272\247\347\233\256\345\275\225", nullptr));
        SaveAndQuit->setText(QApplication::translate("Widget", "\344\277\235\345\255\230", nullptr));
        label->setText(QApplication::translate("Widget", "\345\275\223\345\211\215\350\267\257\345\276\204\357\274\232", nullptr));
        path->setText(QString());
        label_2->setText(QApplication::translate("Widget", "Author\357\274\2321751358 \346\235\216\346\276\216\347\224\237", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
