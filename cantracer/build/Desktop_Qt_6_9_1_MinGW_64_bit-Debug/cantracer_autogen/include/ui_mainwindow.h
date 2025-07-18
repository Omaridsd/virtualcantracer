/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btnloadjson;
    QPushButton *btnloadasc;
    QTableView *tableviewsignals;
    QLabel *labelstatus;
    QTextEdit *texteditlogs;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btnloadjson = new QPushButton(centralwidget);
        btnloadjson->setObjectName("btnloadjson");
        btnloadjson->setGeometry(QRect(230, 270, 75, 24));
        btnloadasc = new QPushButton(centralwidget);
        btnloadasc->setObjectName("btnloadasc");
        btnloadasc->setGeometry(QRect(710, 260, 75, 24));
        tableviewsignals = new QTableView(centralwidget);
        tableviewsignals->setObjectName("tableviewsignals");
        tableviewsignals->setGeometry(QRect(210, 40, 581, 192));
        tableviewsignals->setAlternatingRowColors(true);
        labelstatus = new QLabel(centralwidget);
        labelstatus->setObjectName("labelstatus");
        labelstatus->setGeometry(QRect(380, 10, 151, 16));
        labelstatus->setTextFormat(Qt::TextFormat::PlainText);
        texteditlogs = new QTextEdit(centralwidget);
        texteditlogs->setObjectName("texteditlogs");
        texteditlogs->setGeometry(QRect(0, 20, 201, 71));
        texteditlogs->setReadOnly(true);
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnloadjson->setText(QCoreApplication::translate("MainWindow", "charger json", nullptr));
        btnloadasc->setText(QCoreApplication::translate("MainWindow", "charger asc", nullptr));
        labelstatus->setText(QCoreApplication::translate("MainWindow", "can bus tracer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
