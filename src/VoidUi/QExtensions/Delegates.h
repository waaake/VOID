// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_Q_EXT_DELEGATES_H
#define _VOID_Q_EXT_DELEGATES_H

/* Qt */
#include <QStyledItemDelegate>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class HCustomItemDelegate : public QStyledItemDelegate
{
public:
    HCustomItemDelegate(int height, QObject* parent = nullptr);
    virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

private:
    int m_Height;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_Q_EXT_DELEGATES_H
