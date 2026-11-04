// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SPREADSHEET_COLORBOX_DELEGATE_H
#define _SPREADSHEET_COLORBOX_DELEGATE_H

/* Qt */
#include <QStyledItemDelegate>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class ColorBoxDelegate : public QStyledItemDelegate
{
public:
    explicit ColorBoxDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _SPREADSHEET_COLORBOX_DELEGATE_H
