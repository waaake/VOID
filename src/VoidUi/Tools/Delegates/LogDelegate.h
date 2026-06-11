// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _LOGGING_DELEGATE_H
#define _LOGGING_DELEGATE_H

/* Qt */
#include <QStyledItemDelegate>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class ColoredLogDelegate : public QStyledItemDelegate
{
public:
    explicit ColoredLogDelegate(QObject* parent = nullptr);
    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _LOGGING_DELEGATE_H
