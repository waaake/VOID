// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VERSION_DELEGATE_H
#define _VERSION_DELEGATE_H

/* Qt */
#include <QStyledItemDelegate>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class MediaVersionDelegate : public QStyledItemDelegate
{
public:
    explicit MediaVersionDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _VERSION_DELEGATE_H
