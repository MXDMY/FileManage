/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionSetRootDir;
    QAction *actionShowHideTreeCol_1;
    QAction *actionShowHideTreeCol_2;
    QAction *actionShowHideTreeCol_3;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_3;
    QListView *listView;
    QTreeView *treeView;
    QVBoxLayout *verticalLayout_2;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit;
    QLabel *label_2;
    QLineEdit *lineEdit_2;
    QTabWidget *tabWidget_2;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_4;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QCheckBox *checkBox;
    QToolBar *toolBar;
    QMenuBar *menuBar;
    QMenu *menu;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(802, 510);
        actionSetRootDir = new QAction(MainWindow);
        actionSetRootDir->setObjectName(QStringLiteral("actionSetRootDir"));
        actionShowHideTreeCol_1 = new QAction(MainWindow);
        actionShowHideTreeCol_1->setObjectName(QStringLiteral("actionShowHideTreeCol_1"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icon/sizeColumnGrey.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShowHideTreeCol_1->setIcon(icon);
        actionShowHideTreeCol_2 = new QAction(MainWindow);
        actionShowHideTreeCol_2->setObjectName(QStringLiteral("actionShowHideTreeCol_2"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icon/typeColumnGrey.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShowHideTreeCol_2->setIcon(icon1);
        actionShowHideTreeCol_3 = new QAction(MainWindow);
        actionShowHideTreeCol_3->setObjectName(QStringLiteral("actionShowHideTreeCol_3"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icon/modifyTimeColumnGrey.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionShowHideTreeCol_3->setIcon(icon2);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        horizontalLayout = new QHBoxLayout(tab);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(6, 6, 6, 6);
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        listView = new QListView(tab);
        listView->setObjectName(QStringLiteral("listView"));

        verticalLayout_3->addWidget(listView);

        treeView = new QTreeView(tab);
        treeView->setObjectName(QStringLiteral("treeView"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(treeView->sizePolicy().hasHeightForWidth());
        treeView->setSizePolicy(sizePolicy);

        verticalLayout_3->addWidget(treeView);

        verticalLayout_3->setStretch(0, 1);
        verticalLayout_3->setStretch(1, 5);

        horizontalLayout->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        frame = new QFrame(tab);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout_2 = new QHBoxLayout(frame);
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(frame);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        lineEdit = new QLineEdit(frame);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        horizontalLayout_2->addWidget(lineEdit);

        label_2 = new QLabel(frame);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEdit_2 = new QLineEdit(frame);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));

        horizontalLayout_2->addWidget(lineEdit_2);


        verticalLayout_2->addWidget(frame);

        tabWidget_2 = new QTabWidget(tab);
        tabWidget_2->setObjectName(QStringLiteral("tabWidget_2"));

        verticalLayout_2->addWidget(tabWidget_2);


        horizontalLayout->addLayout(verticalLayout_2);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        verticalLayout_4 = new QVBoxLayout(tab_2);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(6, 6, 6, 6);
        scrollArea = new QScrollArea(tab_2);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 784, 387));
        checkBox = new QCheckBox(scrollAreaWidgetContents);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        checkBox->setGeometry(QRect(10, 10, 131, 18));
        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_4->addWidget(scrollArea);

        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);

        MainWindow->setCentralWidget(centralWidget);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 802, 22));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        toolBar->addAction(actionShowHideTreeCol_1);
        toolBar->addAction(actionShowHideTreeCol_2);
        toolBar->addAction(actionShowHideTreeCol_3);
        menuBar->addAction(menu->menuAction());
        menu->addAction(actionSetRootDir);

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);
        tabWidget_2->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionSetRootDir->setText(QApplication::translate("MainWindow", "\350\256\276\347\275\256\346\240\271\347\233\256\345\275\225", nullptr));
#ifndef QT_NO_TOOLTIP
        actionSetRootDir->setToolTip(QApplication::translate("MainWindow", "\350\256\276\347\275\256\346\240\271\347\233\256\345\275\225", nullptr));
#endif // QT_NO_TOOLTIP
        actionShowHideTreeCol_1->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272/\351\232\220\350\227\217\346\226\207\344\273\266\345\244\247\345\260\217", nullptr));
#ifndef QT_NO_TOOLTIP
        actionShowHideTreeCol_1->setToolTip(QApplication::translate("MainWindow", "\346\230\276\347\244\272/\351\232\220\350\227\217\346\226\207\344\273\266\345\244\247\345\260\217", nullptr));
#endif // QT_NO_TOOLTIP
        actionShowHideTreeCol_2->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272/\351\232\220\350\227\217\346\226\207\344\273\266\347\261\273\345\236\213", nullptr));
#ifndef QT_NO_TOOLTIP
        actionShowHideTreeCol_2->setToolTip(QApplication::translate("MainWindow", "\346\230\276\347\244\272/\351\232\220\350\227\217\346\226\207\344\273\266\347\261\273\345\236\213", nullptr));
#endif // QT_NO_TOOLTIP
        actionShowHideTreeCol_3->setText(QApplication::translate("MainWindow", "\346\230\276\347\244\272/\351\232\220\350\227\217\346\226\207\344\273\266\344\277\256\346\224\271\346\227\266\351\227\264", nullptr));
#ifndef QT_NO_TOOLTIP
        actionShowHideTreeCol_3->setToolTip(QApplication::translate("MainWindow", "\346\230\276\347\244\272/\351\232\220\350\227\217\346\226\207\344\273\266\344\277\256\346\224\271\346\227\266\351\227\264", nullptr));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        label_2->setText(QApplication::translate("MainWindow", "TextLabel", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MainWindow", "Tab 1", nullptr));
        checkBox->setText(QApplication::translate("MainWindow", " \344\277\235\345\255\230\346\240\271\347\233\256\345\275\225\345\210\227\350\241\250", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MainWindow", "Tab 2", nullptr));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", nullptr));
        menu->setTitle(QApplication::translate("MainWindow", "\350\217\234\345\215\225", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
