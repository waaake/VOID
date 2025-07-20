#ifndef _VOID_MEDIA_LIST_DELEGATE_H
#define _VOID_MEDIA_LIST_DELEGATE_H

/* Qt */
#include <QStyledItemDelegate>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class MediaItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit MediaItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_LIST_DELEGATE_H
