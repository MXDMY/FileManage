/*
 * 该类继承自QThred，用于将一个文件路径列表里对应的文件、目录复制到一个新的目录
 * 调用该线程的start()函数前，请先调用公共函数setDestination、setSource、
 * setProgressDialog，分别传入线程使用所需要的新目录，待复制的文件路径列表以
 * 及进度条对话框
*/
#ifndef FMFILESYSTEMCOPYTHREAD_H
#define FMFILESYSTEMCOPYTHREAD_H

#include "progressdialog.h"

#include <QThread>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QMessageBox>

class FMFileSystemCopyThread : public QThread
{
    Q_OBJECT

public:
    explicit FMFileSystemCopyThread(QObject* parent=0);

    void setDestination(QString newDir);

    void setSource(QStringList pathList);

    void setProgressDialog(ProgressDialog* progressDialog);

protected:
    void run();

signals:
    //当复制操作检测到同名文件、目录时，发出此信号给主线程，让主线程返回用户选择
    int whetherToReplace(QString path);

private:
    void copyDir(QString dir);              //传递给该函数的dir以及私用变量newDir必须是有效的

    void copyFile(QString file);            //传递给该函数的file以及私用变量newDir必须是有效的

    bool delDir(QString dir);               //传递给该函数的dir必须是有效的，且不为操作系统根目录

private:
    QString newDir;                         //目的地目录，文件分隔符应该为QT格式
    QStringList pathList;                   //待复制的文件和目录，文件分隔符应该为QT格式
    ProgressDialog* progressDialog=NULL;    //进度对话框指针
};

#endif // FMFILESYSTEMCOPYTHREAD_H
