#include "fmfilesystemmodel.h"

FMFileSystemModel::FMFileSystemModel(QObject *parent) : QFileSystemModel(parent)
{

}

void FMFileSystemModel::setProgressDialog(ProgressDialog *progressDialog)
{
    this->progressDialog=progressDialog;
}

void FMFileSystemModel::setFileSysCpThread(FMFileSystemCopyThread *fileSysCpThread)
{
    this->fileSysCpThread=fileSysCpThread;
}

QVariant FMFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(section==0&&role==Qt::DisplayRole&&orientation==Qt::Horizontal)
        return "文件名";
    if(section==1&&role==Qt::DisplayRole&&orientation==Qt::Horizontal)
        return "文件大小";
    if(section==2&&role==Qt::DisplayRole&&orientation==Qt::Horizontal)
        return "文件类型";
    if(section==3&&role==Qt::DisplayRole&&orientation==Qt::Horizontal)
        return "上次修改时间";
    return QFileSystemModel::headerData(section,orientation,role);
}

Qt::ItemFlags FMFileSystemModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags=QFileSystemModel::flags(index);//获取父类函数返回
    QFileInfo fileInfo=this->fileInfo(index);
    if(index.isValid())
    {
        if(fileInfo.isFile()||fileInfo.isSymLink())
        {
            return defaultFlags|Qt::ItemIsDragEnabled;
        }
        else
        {
            return defaultFlags|Qt::ItemIsDropEnabled|Qt::ItemIsDragEnabled;
        }
    }
    else
        return defaultFlags|Qt::ItemIsDropEnabled;//无效item也可作为drop目标
}

bool FMFileSystemModel::canDropMimeData(const QMimeData *data,
                       Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    return QFileSystemModel::canDropMimeData(data,action,row,column,parent);
}

bool FMFileSystemModel::dropMimeData(const QMimeData *data,
                       Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    //row与column、parent指明拖放操作结束的位置
    //return QFileSystemModel::dropMimeData(data,action,row,column,parent);
    //重写，以通过拖放完成复制
    QString newDir=this->fileInfo(parent).absoluteFilePath();
    if(newDir.isEmpty())
        return QFileSystemModel::dropMimeData(data,action,row,column,parent);
    QStringList pathList;
    QList<QUrl> urls=data->urls();
    for(int i=0;i<urls.count();i++)
    {
        if(urls.at(i).isLocalFile())
            pathList.append(urls.at(i).toLocalFile());
    }

    fileSysCpThread->setDestination(newDir);
    fileSysCpThread->setSource(pathList);
    fileSysCpThread->setProgressDialog(progressDialog);

    progressDialog->setProgressThreadType(1);

    fileSysCpThread->start();
    progressDialog->show();
    return true;
}

Qt::DropActions FMFileSystemModel::supportedDropActions() const
{
    //return Qt::CopyAction | Qt::MoveAction | Qt::LinkAction;
    return QFileSystemModel::supportedDropActions();
}
