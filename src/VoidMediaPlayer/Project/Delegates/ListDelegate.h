// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PROJECT_LIST_DELEGATE_H
#define _VOID_PROJECT_LIST_DELEGATE_H

/* Qt */
#include <QStyledItemDelegate>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class ProjectItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit ProjectItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class RecentProjectItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit RecentProjectItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_LIST_DELEGATE_H
