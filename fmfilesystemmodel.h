#ifndef FMFILESYSTEMMODEL_H
#define FMFILESYSTEMMODEL_H

#include "fmfilesystemcopythread.h"
#include "progressdialog.h"

#include <QFileSystemModel>
#include <QMimeData>
#include <QUrl>

/* Ui {
class FMFileSystemModel;
}*/

class FMFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    explicit FMFileSystemModel(QObject *parent = 0);

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool canDropMimeData(const QMimeData *data,
                         Qt::DropAction action, int row, int column, const QModelIndex &parent) const;

    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

    Qt::DropActions supportedDropActions() const;

    //以下为自定义函数
    void setProgressDialog(ProgressDialog* progressDialog);
    void setFileSysCpThread(FMFileSystemCopyThread* fileSysCpThread);

private:
    ProgressDialog* progressDialog=NULL;
    FMFileSystemCopyThread* fileSysCpThread=NULL;

};

#endif // FMFILESYSTEMMODEL_H
