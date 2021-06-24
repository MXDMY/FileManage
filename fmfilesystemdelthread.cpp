#include "fmfilesystemdelthread.h"

FMFileSystemDelThread::FMFileSystemDelThread(QObject* parent) : QThread(parent)
{

}

void FMFileSystemDelThread::setPathList(QStringList pathList)
{
    this->pathList=pathList;
}

void FMFileSystemDelThread::setProgressDialog(ProgressDialog *progressDialog)
{
    this->progressDialog=progressDialog;
}

void FMFileSystemDelThread::run()
{
    if(progressDialog==NULL)
    {
        //未获得文件对话框指针，直接退出，不发出任何报错信息
        return;
    }

    QString path;
    QFileInfo fileInfo;
    for(int i=0;i<pathList.count();i++)
    {
        path=pathList.at(i);
        fileInfo=QFileInfo(path);
        if(fileInfo.exists())
        {
            if(fileInfo.isFile()||fileInfo.isSymLink())
            {
                if(!QFile::remove(path))
                {
                    progressDialog->appendText(QDir::toNativeSeparators(path)+"::删除失败");
                    progressDialog->appendText("=========================");
                }
            }
            else if(fileInfo.isRoot())
            {
                progressDialog->appendText(QDir::toNativeSeparators(path)+"::检测为操作系统根目录，不可删除");
                progressDialog->appendText("=========================");
            }
            else
            {
                if(!delDir(path))
                {
                    progressDialog->appendText(QDir::toNativeSeparators(path)+"::删除失败");
                    progressDialog->appendText("=========================");
                }
            }
        }
        else
        {
            progressDialog->appendText(QDir::toNativeSeparators(path)+"::已不存在");
            progressDialog->appendText("=========================");
        }
    }
}

bool FMFileSystemDelThread::delDir(QString dir)
{
    QStringList unSearched;     //储存未被搜索的文件、目录
    QStringList unDealt;        //储存未被处理的文件、目录
    unSearched.append(dir);
    QString temp;
    QFileInfo fileInfo;
    while(!unSearched.empty())
    {
        //提取unSearched首部元素
        temp=unSearched.at(0);
        unSearched.removeAt(0);
        fileInfo=QFileInfo(temp);
        unDealt.append(temp);
        if(fileInfo.isDir() && (!fileInfo.isSymLink()))
        {
            QFileInfoList fileInfoList=QDir(temp).entryInfoList(
                        QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden);
            for(int i=0;i<fileInfoList.count();i++)
            {
                //元素添加进unSearched尾部，这种方式可以保证unDealt内的文件路径父项在前，子项在后
                unSearched.append(fileInfoList.at(i).absoluteFilePath());
            }
        }
    }

    for(int i=unDealt.count()-1;i>=0;i--)
    {
        temp=unDealt.at(i);
        fileInfo=QFileInfo(temp);
        if(fileInfo.isFile()||fileInfo.isSymLink())
        {
            if(!QFile::remove(temp))
            {
                progressDialog->appendText(QDir::toNativeSeparators(temp)+"::删除失败");
                progressDialog->appendText("=========================");
                return false;
            }
        }
        else
        {
            if(!fileInfo.absoluteDir().rmdir(fileInfo.fileName()))
            {
                progressDialog->appendText(QDir::toNativeSeparators(temp)+"::删除失败");
                progressDialog->appendText("=========================");
                return false;
            }
        }
    }
    return true;
}
