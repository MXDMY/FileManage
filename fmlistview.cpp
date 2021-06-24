#include "fmlistview.h"

FMListView::FMListView(QWidget* parent) : QListView(parent)
{
    //this->setAcceptDrops(true);
}

void FMListView::dragEnterEvent(QDragEnterEvent *event)
{
    QListView::dragEnterEvent(event);
}

void FMListView::dragMoveEvent(QDragMoveEvent *event)
{
    QListView::dragMoveEvent(event);
}
