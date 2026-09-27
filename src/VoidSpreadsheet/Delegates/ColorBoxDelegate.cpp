// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "ColorBoxDelegate.h"
#include "VoidSpreadsheet/Models/Spreadsheet.h"

VOID_NAMESPACE_OPEN

ColorBoxDelegate::ColorBoxDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void ColorBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    float hcenter = (float)option.rect.width() / 2;
    float vcenter = (float)option.rect.height() / 2;
    QRect r(option.rect.left() + hcenter - 6, option.rect.top() + vcenter - 6, 12, 12);
    painter->fillRect(r, index.data(static_cast<int>(SequenceItemsModel::Roles::Color)).value<QColor>());
}

VOID_NAMESPACE_CLOSE
