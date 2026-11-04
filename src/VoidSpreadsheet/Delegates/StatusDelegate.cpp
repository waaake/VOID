// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "StatusDelegate.h"
#include "VoidSpreadsheet/Models/Spreadsheet.h"

VOID_NAMESPACE_OPEN

LinkStatusDelegate::LinkStatusDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void LinkStatusDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    float hcenter = (float)option.rect.width() / 2;
    float vcenter = (float)option.rect.height() / 2;
    QRect r(option.rect.left() + hcenter - 10, option.rect.top() + vcenter - 6, 20, 12);

    painter->fillRect(
        r, 
        index.data(static_cast<int>(SequenceItemsModel::Roles::Status)).value<bool>()
            ? QColor(50, 160, 70)
            : QColor(160, 70, 50)
    );
}

VOID_NAMESPACE_CLOSE
