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
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionChargerDBC;
    QAction *actionChargerASC;
    QAction *actionChangeGraphColor;
    QAction *actionResetZoom;
    QAction *btnActivateCursors;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QTreeWidget *treeSignals;
    QLabel *labelStatus;
    QScrollArea *scrollAreaGraphs;
    QWidget *scrollContentWidget;
    QVBoxLayout *verticalLayout_3;
    QVBoxLayout *graphsLayout;
    QStatusBar *statusBar;
    QToolBar *toolBar;
    QMenuBar *menubar;
    QMenu *menufenetre;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(976, 426);
        actionChargerDBC = new QAction(MainWindow);
        actionChargerDBC->setObjectName("actionChargerDBC");
        actionChargerDBC->setMenuRole(QAction::MenuRole::NoRole);
        actionChargerASC = new QAction(MainWindow);
        actionChargerASC->setObjectName("actionChargerASC");
        actionChargerASC->setMenuRole(QAction::MenuRole::NoRole);
        actionChangeGraphColor = new QAction(MainWindow);
        actionChangeGraphColor->setObjectName("actionChangeGraphColor");
        actionChangeGraphColor->setMenuRole(QAction::MenuRole::NoRole);
        actionResetZoom = new QAction(MainWindow);
        actionResetZoom->setObjectName("actionResetZoom");
        actionResetZoom->setMenuRole(QAction::MenuRole::NoRole);
        btnActivateCursors = new QAction(MainWindow);
        btnActivateCursors->setObjectName("btnActivateCursors");
        btnActivateCursors->setMenuRole(QAction::MenuRole::NoRole);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        centralwidget->setAutoFillBackground(false);
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setSpacing(5);
        horizontalLayout->setObjectName("horizontalLayout");
        horizontalLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setSizeConstraint(QLayout::SizeConstraint::SetDefaultConstraint);
        treeSignals = new QTreeWidget(centralwidget);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        treeSignals->setHeaderItem(__qtreewidgetitem);
        treeSignals->setObjectName("treeSignals");
        treeSignals->setMaximumSize(QSize(250, 16777215));
        treeSignals->setUniformRowHeights(true);
        treeSignals->setHeaderHidden(true);

        verticalLayout->addWidget(treeSignals);

        labelStatus = new QLabel(centralwidget);
        labelStatus->setObjectName("labelStatus");
        labelStatus->setTextFormat(Qt::TextFormat::PlainText);

        verticalLayout->addWidget(labelStatus);


        horizontalLayout->addLayout(verticalLayout);

        scrollAreaGraphs = new QScrollArea(centralwidget);
        scrollAreaGraphs->setObjectName("scrollAreaGraphs");
        scrollAreaGraphs->setWidgetResizable(true);
        scrollContentWidget = new QWidget();
        scrollContentWidget->setObjectName("scrollContentWidget");
        scrollContentWidget->setGeometry(QRect(0, 0, 474, 337));
        verticalLayout_3 = new QVBoxLayout(scrollContentWidget);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setObjectName("verticalLayout_3");
        verticalLayout_3->setContentsMargins(2, 2, 2, 2);
        graphsLayout = new QVBoxLayout();
        graphsLayout->setSpacing(6);
        graphsLayout->setObjectName("graphsLayout");

        verticalLayout_3->addLayout(graphsLayout);

        scrollAreaGraphs->setWidget(scrollContentWidget);

        horizontalLayout->addWidget(scrollAreaGraphs);

        MainWindow->setCentralWidget(centralwidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName("statusBar");
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName("toolBar");
        MainWindow->addToolBar(Qt::ToolBarArea::TopToolBarArea, toolBar);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 976, 22));
        menufenetre = new QMenu(menubar);
        menufenetre->setObjectName("menufenetre");
        MainWindow->setMenuBar(menubar);

        toolBar->addSeparator();
        toolBar->addAction(actionChargerDBC);
        toolBar->addSeparator();
        toolBar->addAction(actionChargerASC);
        toolBar->addAction(actionChangeGraphColor);
        toolBar->addAction(actionResetZoom);
        toolBar->addAction(btnActivateCursors);
        menubar->addAction(menufenetre->menuAction());
        menufenetre->addSeparator();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        actionChargerDBC->setText(QCoreApplication::translate("MainWindow", "select DBC", nullptr));
        actionChargerASC->setText(QCoreApplication::translate("MainWindow", "select ASC", nullptr));
        actionChangeGraphColor->setText(QCoreApplication::translate("MainWindow", "color", nullptr));
        actionResetZoom->setText(QCoreApplication::translate("MainWindow", "zoom", nullptr));
        btnActivateCursors->setText(QCoreApplication::translate("MainWindow", "Activer curseurs", nullptr));
        labelStatus->setText(QString());
        toolBar->setWindowTitle(QCoreApplication::translate("MainWindow", "toolBar", nullptr));
        menufenetre->setTitle(QCoreApplication::translate("MainWindow", "fenetre", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
