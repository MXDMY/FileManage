/*
 * 本想重写该类，重新实现软件右侧listView，以贴合预想的拖放设计，后来一想，
 * QT的view与model配套实现，想重写QListView，就得查看源码，在重写的过程中
 * 考虑QFileSystemModel的相关函数，工作量大，又因本人太懒，所以干脆先放着，
 * 以后有心情再写
*/

#ifndef FMLISTVIEW_H
#define FMLISTVIEW_H

#include <QListView>
#include <QDragEnterEvent>
#include <QMimeData>

class FMListView : public QListView
{
    Q_OBJECT

public:
    explicit FMListView(QWidget* parent=0);

protected:
    void dragEnterEvent(QDragEnterEvent *event);

    void dragMoveEvent(QDragMoveEvent* event);
};

#endif // FMLISTVIEW_H
