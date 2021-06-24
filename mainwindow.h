#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "progressdialog.h"
#include "fmfilesystemmodel.h"
#include "fmfilesystemcopythread.h"
#include "fmfilesystemdelthread.h"
#include "fmlistview.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QFileInfo>
#include <QFileIconProvider>
#include <QDir>
//#include <QMap>
#include <QMouseEvent>
//#include <QListView>
#include <QMenu>
#include <QAction>
#include <QMimeData>
#include <QClipboard>
#include <QUrl>
#include <QVariant>
#include <QDesktopServices>
#include <QInputDialog>
#include <QToolButton>
//#include <QMessageBox>
//#include <QFileSystemWatcher>
#include <QSystemTrayIcon>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();   

protected:
    bool eventFilter(QObject *watched, QEvent *event);  //事件过滤器

    void closeEvent(QCloseEvent *event);                //重写关闭事件的处理，以便退出时保存相关配置

private slots:
    void on_actionSetRootDir_triggered();       //选择并设置一个根目录到目录树列表

    void on_treeView_clicked(const QModelIndex &index);

    void on_treeView_doubleClicked(const QModelIndex &index);

    void listViewClickedSlot(const QModelIndex &index);  //处理在tabWidget_2的tab页（类型为QListView）的单击信号

    void listViewDoubleClickedSlot(const QModelIndex &index);//和上述类似，不同的是这里处理双击信号

    void showTreeViewMenu(QPoint pos);

    void showListViewMenu(QPoint pos);

    void showRootListMenu(QPoint pos);

    void trayIconQuitSlot(bool checked);        //系统托盘菜单退出动作响应

    void treeViewCopySlot(bool checked);        //treeView菜单复制动作响应

    void treeViewDelSlot(bool checked);         //treeView菜单删除动作响应

    void treeViewPasteSlot(bool checked);       //treeView菜单粘贴动作响应，将剪切板的文件、目录粘贴在同一级

    void treeViewShowInExplorerSlot(bool checked); //在资源管理器中显示对应项

    void treeViewRenameSlot(bool checked);      //treeView菜单重命名响应

    void listViewCopySlot(bool checked);        //listView菜单复制动作响应

    void listViewDelSlot(bool checked);         //listView菜单删除动作响应

    void listViewPasteSlot(bool checked);       //listView菜单粘贴动作响应，将剪切板的文件、目录粘贴在同一级

    void listViewShowInExplorerSlot(bool checked); //在资源管理器中显示对应项

    void listViewRenameSlot(bool checked);      //listView菜单重命名响应

    void tabCloseRequestedSlot(int index);      //tabWidget_2的Tab关闭按钮响应

    void progressInterrupted();                 //进度条对话框中断操作响应

    void fileSysCpFinished();                   //文件系统复制线程结束响应

    void fileSysDelFinished();                  //文件系统删除线程结束响应

    int whetherToReplaceSlot(QString path);     //文件系统复制线程检测到同名文件时需要获取用户选择的响应

    void tabWidget2AddTab();                    //tabWidget_2新增一页

    void tabWidget2Back();                      //让当前页所展示的目录返回上一级

    //根据path路径，更新tabWidget_2指定的页；path可以对应文件，若对应文件，则更新会根据该文件所在的目录进行处理
    void tabWidget2UpdateTab(QString path,int tabIndex);

    void on_listView_clicked(const QModelIndex &index);

    void rootListRemoveSlot(bool checked);      //移出根目录列表的当前项

    void on_actionShowHideTreeCol_1_triggered();

    void on_actionShowHideTreeCol_2_triggered();

    void on_actionShowHideTreeCol_3_triggered();

    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    void on_checkBox_stateChanged(int arg1);

private:
    void initWidgetSetting();                   //初始化各个控件设置

    void setFromINI();                          //根据配置文件进行设置

    QStandardItem* rootListAddItem(QString rootDir);      //向根目录列表添加新项，并返回新项

    void fmCopy(QStringList pathList);
    void fmDel(QStringList pathList);
    void fmPaste(QString newDir);
    void fmShowInExplorer(QString path);
    void fmRename(QString path);

    //判断当前鼠标位置是否位于treeView与tabWidget_2对应的边缘的拖动区域
    bool isDragRegion(QMouseEvent* e);
    //判断当前鼠标位置是否位于treeView与ui->listView对应的边缘的拖动区域
    bool isDragRegion_2(QMouseEvent* e);

    void initTreeViewMenu();                    //初始化treeView的菜单
    void initListViewMenu();                    //初始化listView的菜单
    void initRootListMenu();                    //初始化ui->listView（根目录列表）的菜单
    void initTrayIconMenu();                        //初始化系统托盘图标菜单

private:
    Ui::MainWindow *ui;

    QSettings* setting=NULL;                    //程序与配置文件的接口

    QSystemTrayIcon* trayIcon=NULL;             //系统托盘图标
    QMenu* trayIconMenu=NULL;
    QAction* trayIconQuit=NULL;

    QStandardItemModel* rootListModel=NULL;     //根目录listView所使用的模型

    QMenu* rootListMenu=NULL;
    QAction* rootListRemove=NULL;

    QToolButton* tabWidget2AddTabButton=NULL;
    QToolButton* tabWidget2BackButton=NULL;

    //对话框与下列线程的信号的处理都在mainwindow进行，其它涉及使用 对话框与下列线程 的类只负责引用
    ProgressDialog* progressDialog=NULL;
    FMFileSystemCopyThread* fileSysCpThread=NULL;
    FMFileSystemDelThread* fileSysDelThread=NULL;

    FMFileSystemModel* treeModel=NULL;

    QClipboard* clipBoard=NULL;

    QPoint treeViewMenuCoo;                     //存放treeView请求上下文菜单事件发出的坐标
    QMenu* treeViewMenu=NULL;
    QAction* treeViewCopy=NULL;
    QAction* treeViewDel=NULL;
    QAction* treeViewPaste=NULL;
    QAction* treeViewShowInExplorer=NULL;
    QAction* treeViewRename=NULL;

    QPoint listViewMenuCoo;                     //存放listView请求上下文菜单事件发出的坐标
    QMenu* listViewMenu=NULL;
    QAction* listViewCopy=NULL;
    QAction* listViewDel=NULL;
    QAction* listViewPaste=NULL;
    QAction* listViewShowInExplorer=NULL;
    QAction* listViewRename=NULL;
};

#endif // MAINWINDOW_H
