/*
 * 该类继承自QThred，用于将一个文件路径列表里对应的文件、目录全部删除
 * 调用该线程的start()函数前，请先调用公共函数setPathList、
 * setProgressDialog，分别传入线程使用所需要删除的文件路径列表以
 * 及进度条对话框
*/

#ifndef FMFILESYSTEMDELTHREAD_H
#define FMFILESYSTEMDELTHREAD_H

#include "progressdialog.h"

#include <QThread>
#include <QFileInfo>
#include <QDir>
#include <QFile>

class FMFileSystemDelThread : public QThread
{
    Q_OBJECT

public:
    explicit FMFileSystemDelThread(QObject* parent=0);

    void setPathList(QStringList pathList);

    void setProgressDialog(ProgressDialog* progressDialog);

protected:
    void run();

private:
    bool delDir(QString dir);               //传递给该函数的dir必须是有效的，且不为操作系统根目录

private:
    QStringList pathList;                   //待删除的文件和目录，文件分隔符应该为QT格式
    ProgressDialog* progressDialog=NULL;    //进度对话框指针
};

#endif // FMFILESYSTEMDELTHREAD_H
