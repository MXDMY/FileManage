#include "fmfilesystemcopythread.h"

FMFileSystemCopyThread::FMFileSystemCopyThread(QObject* parent) : QThread(parent)
{
    //由于用户选择采用QMessageBox，但QMessageBox只能在主线程执行，因此，需要连接信号到主线程，
    //QMessageBox在主线程执行，连接方式为阻塞模式
    connect(this,SIGNAL(whetherToReplace(QString)),
            this->parent(),SLOT(whetherToReplaceSlot(QString)),Qt::BlockingQueuedConnection);
}

void FMFileSystemCopyThread::run()
{
    if(progressDialog==NULL)
    {
        //未获得文件对话框指针，直接退出，不发出任何报错信息
        return;
    }
    QFileInfo fileInfo(newDir);
    if(fileInfo.isFile() || fileInfo.isSymLink() || (!fileInfo.exists()) )
    {
        //目的地目录无效，直接退出，不发出任何报错信息
        return;
    }

    QString path;
    while(!pathList.empty())
    {
        path=pathList.at(0);
        pathList.removeAt(0);

        fileInfo=QFileInfo(path);
        //path对应的文件或目录不存在的处理方式
        if(!fileInfo.exists())
        {
            progressDialog->appendText(QDir::toNativeSeparators(path)+"::不存在，已跳过");
            progressDialog->appendText("=========================");
        }
        //path对应文件的处理方式
        else if(fileInfo.isFile()||fileInfo.isSymLink())
        {
            progressDialog->appendText(QDir::toNativeSeparators(path)+"::文件开始复制");
            progressDialog->appendText("=========================");
            copyFile(path);
        }
        //path对应目录的处理方式
        else
        {
            //path对应操作系统根目录的处理方式
            if(fileInfo.isRoot())
            {
                progressDialog->appendText(QDir::toNativeSeparators(path)+"::检测为操作系统根目录，将靠后处理其子项");
                progressDialog->appendText("=========================");
                QFileInfoList fileInfoList=QDir(path).entryInfoList(
                            QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Hidden);
                for(int i=0;i<fileInfoList.count();i++)
                    pathList.append(fileInfoList.at(i).absoluteFilePath());
            }
            //path对应普通目录的处理方式
            else
            {
                progressDialog->appendText(QDir::toNativeSeparators(path)+"::目录开始复制");
                progressDialog->appendText("=========================");
                copyDir(path);
            }
        }
    }
}

void FMFileSystemCopyThread::setDestination(QString newDir)
{
    this->newDir=newDir;
}

void FMFileSystemCopyThread::setSource(QStringList pathList)
{
    this->pathList=pathList;
}

void FMFileSystemCopyThread::setProgressDialog(ProgressDialog *progressDialog)
{
    this->progressDialog=progressDialog;
}

void FMFileSystemCopyThread::copyDir(QString dir)
{
    int len=QFileInfo(dir).absolutePath().length();//拿到dir的父目录的文件路径长度（单位为字符个数）
    QString dir2=dir;
    dir2.replace(0,len,newDir+"/");
    if(QFileInfo(dir).absoluteFilePath()==QFileInfo(dir2).absoluteFilePath())
    {
        progressDialog->appendText(QDir::toNativeSeparators(dir)+"::源与目的地相同，跳过");
        progressDialog->appendText("=========================");
        return;
    }
    if(QFileInfo(dir2).exists())
    {
        progressDialog->appendText(QDir::toNativeSeparators(dir)+"::检测到同名目录，等待用户选择");
        progressDialog->appendText("=========================");
        int choice=whetherToReplace(dir2);//发送信号给主线程，并获得用户选择返回值，此处省略了emit
        if(choice==QMessageBox::Ok)
        {
            progressDialog->appendText(QDir::toNativeSeparators(dir)+"::用户选择替换");
            progressDialog->appendText("=========================");
            if(!delDir(dir2))
            {
                progressDialog->appendText(QDir::toNativeSeparators(dir)+"::同名目录删除失败，无法复制");
                progressDialog->appendText("=========================");
                return;
            }
        }
        else
        {
            progressDialog->appendText(QDir::toNativeSeparators(dir)+"::用户选择不替换，跳过");
            progressDialog->appendText("=========================");
            return;
        }
    }

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

    QString S;
    QString D;
    for(int i=0;i<unDealt.count();i++)
    {
        S=unDealt.at(i);
        D=S;
        D.replace(0,len,newDir+"/");    //替换对应字符，将D换算为新的子项路径
        fileInfo=QFileInfo(S);
        if(fileInfo.isFile()||fileInfo.isSymLink())
        {
            if(QFile::copy(S,D))
            {
                progressDialog->appendText(QDir::toNativeSeparators(S)+"::复制成功");
                progressDialog->appendText("=========================");
            }
            else
            {
                progressDialog->appendText(QDir::toNativeSeparators(S)+"::复制失败");
                progressDialog->appendText("=========================");
            }
        }
        else
        {
            if(QFileInfo(D).absoluteDir().mkdir(fileInfo.fileName()))
            {
                progressDialog->appendText(QDir::toNativeSeparators(S)+"::创建成功");
                progressDialog->appendText("=========================");
            }
            else
            {
                progressDialog->appendText(QDir::toNativeSeparators(S)+"::创建失败");
                progressDialog->appendText("=========================");
            }
        }
    }
}

void FMFileSystemCopyThread::copyFile(QString file)
{
    QString newFile=newDir+"/"+QFileInfo(file).fileName();
    //QFileInfo("file")，若file为链接文件，则QFileInfo的“==”操作运算符会自己解析链接
    if(QFileInfo(newFile).absoluteFilePath()==QFileInfo(file).absoluteFilePath())
    {
        progressDialog->appendText(QDir::toNativeSeparators(file)+"::源与目的地相同，跳过");
        progressDialog->appendText("=========================");
    }
    else if(QFileInfo(newFile).exists())
    {
        progressDialog->appendText(QDir::toNativeSeparators(file)+"::检测到同名文件，等待用户选择");
        progressDialog->appendText("=========================");
        int choice=whetherToReplace(newFile);//发送信号给主线程，并获得用户选择返回值，此处省略了emit
        if(choice==QMessageBox::Ok)
        {
            progressDialog->appendText(QDir::toNativeSeparators(file)+"::用户选择替换");
            progressDialog->appendText("=========================");
            if(QFile::remove(newFile))
            {
                if(QFile::copy(file,newFile))
                {
                    progressDialog->appendText(QDir::toNativeSeparators(file)+"::复制成功");
                    progressDialog->appendText("=========================");
                }
                else
                {
                    progressDialog->appendText(QDir::toNativeSeparators(file)+"::复制失败");
                    progressDialog->appendText("=========================");
                }
            }
            else
            {
                progressDialog->appendText(QDir::toNativeSeparators(file)+"::同名文件删除失败，无法复制");
                progressDialog->appendText("=========================");
            }
        }
        else
        {
            progressDialog->appendText(QDir::toNativeSeparators(file)+"::用户选择不替换，跳过");
            progressDialog->appendText("=========================");
        }
    }
    else
    {
        if(QFile::copy(file,newFile))
        {
            progressDialog->appendText(QDir::toNativeSeparators(file)+"::复制成功");
            progressDialog->appendText("=========================");
        }
        else
        {
            progressDialog->appendText(QDir::toNativeSeparators(file)+"::复制失败");
            progressDialog->appendText("=========================");
        }
    }
}

bool FMFileSystemCopyThread::delDir(QString dir)
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
