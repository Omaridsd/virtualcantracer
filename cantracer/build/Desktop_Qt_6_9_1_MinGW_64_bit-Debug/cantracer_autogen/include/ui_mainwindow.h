/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCharts/QChartView>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTableView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QChartView *centralwidget;
    QPushButton *btnloaddbc;
    QPushButton *btnloadasc;
    QTableView *tableviewsignals;
    QLabel *labelStatus;
    QChartView *chartContainer;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *btnChooseSignals;
    QMenuBar *menubar;
    QToolBar *toolBar;
    QToolBar *toolBar_2;
    QToolBar *toolBar_3;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(801, 600);
        centralwidget = new QChartView(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setMaximumSize(QSize(790, 16777215));
        btnloaddbc = new QPushButton(centralwidget);
        btnloaddbc->setObjectName("btnloaddbc");
        btnloaddbc->setGeometry(QRect(710, 290, 75, 24));
        btnloadasc = new QPushButton(centralwidget);
        btnloadasc->setObjectName("btnloadasc");
        btnloadasc->setGeometry(QRect(710, 260, 75, 24));
        tableviewsignals = new QTableView(centralwidget);
        tableviewsignals->setObjectName("tableviewsignals");
        tableviewsignals->setGeometry(QRect(210, 40, 561, 201));
        tableviewsignals->setAlternatingRowColors(true);
        labelStatus = new QLabel(centralwidget);
        labelStatus->setObjectName("labelStatus");
        labelStatus->setGeometry(QRect(20, 220, 181, 51));
        labelStatus->setTextFormat(Qt::TextFormat::PlainText);
        chartContainer = new QChartView(centralwidget);
        chartContainer->setObjectName("chartContainer");
        chartContainer->setGeometry(QRect(80, 289, 581, 261));
        verticalLayoutWidget = new QWidget(centralwidget);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(10, 280, 701, 281));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        btnChooseSignals = new QPushButton(centralwidget);
        btnChooseSignals->setObjectName("btnChooseSignals");
        btnChooseSignals->setGeometry(QRect(710, 320, 75, 24));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 801, 22));
        MainWindow->setMenuBar(menubar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName("toolBar");
        MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);
        toolBar_2 = new QToolBar(MainWindow);
        toolBar_2->setObjectName("toolBar_2");
        MainWindow->addToolBar(Qt::ToolBarArea::LeftToolBarArea, toolBar_2);
        toolBar_3 = new QToolBar(MainWindow);
        toolBar_3->setObjectName("toolBar_3");
        MainWindow->addToolBar(Qt::ToolBarArea::LeftToolBarArea, toolBar_3);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnloaddbc->setText(QCoreApplication::translate("MainWindow", "charger dbc", nullptr));
        btnloadasc->setText(QCoreApplication::translate("MainWindow", "charger asc", nullptr));
        labelStatus->setText(QString());
        btnChooseSignals->setText(QCoreApplication::translate("MainWindow", "choisir signal", nullptr));
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
        toolBar_2->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar_2", nullptr));
        toolBar_3->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar_3", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
