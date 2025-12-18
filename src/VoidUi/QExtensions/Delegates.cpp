// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Delegates.h"

VOID_NAMESPACE_OPEN

HCustomItemDelegate::HCustomItemDelegate(int height, QObject* parent)
    : QStyledItemDelegate(parent), m_Height(height)
{
}

QSize HCustomItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(m_Height);
    return size;
}

VOID_NAMESPACE_CLOSE
