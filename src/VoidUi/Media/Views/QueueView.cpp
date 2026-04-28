// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QDropEvent>

/* Internal */
#include "QueueView.h"
#include "VoidUi/Media/Delegates/ListDelegate.h"

VOID_NAMESPACE_OPEN

QueueView::QueueView(QWidget* parent)
    : QListView(parent)
{
    setItemDelegate(new MediaItemDelegate(this));
    setSpacing(1);

    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
}

QueueView::~QueueView()
{
}

void QueueView::Set(Playlist* playlist)
{
    setModel(playlist->DataModel());
}

void QueueView::Clear()
{
    setModel(nullptr);
}

void QueueView::dropEvent(QDropEvent* event)
{
    #if _QT6
    QModelIndex index = indexAt(event->position());
    #else
    QModelIndex index = indexAt(event->pos());
    #endif // _QT6
    int dest = index.isValid() ? index.row() : model()->rowCount();

    // Currently we have single selection only
    int source = currentIndex().row();
    model()->moveRows(QModelIndex(), source, 1, QModelIndex(), dest);
}

VOID_NAMESPACE_CLOSE
