// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SPREADSHEET_STATUS_DELEGATE_H
#define _SPREADSHEET_STATUS_DELEGATE_H

/* Qt */
#include <QStyledItemDelegate>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class LinkStatusDelegate : public QStyledItemDelegate
{
public:
    explicit LinkStatusDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _SPREADSHEET_STATUS_DELEGATE_H
