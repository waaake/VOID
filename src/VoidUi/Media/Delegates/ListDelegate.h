// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_LIST_DELEGATE_H
#define _VOID_MEDIA_LIST_DELEGATE_H

/* Qt */
#include <QStyledItemDelegate>

/* Internal */
#include "QDefinition.h"

VOID_NAMESPACE_OPEN

class BasicMediaItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit BasicMediaItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

signals:
    void tagClicked(const QModelIndex&, const QPoint&);

protected:
    bool editorEvent(QEvent* event, QAbstractItemModel* item, const QStyleOptionViewItem& option, const QModelIndex& index) override;

private:
    mutable int m_TagX, m_TagY;
};

class MediaItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit MediaItemDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;

signals:
    void tagClicked(const QModelIndex&, const QPoint&);

protected:
    bool editorEvent(QEvent* event, QAbstractItemModel* item, const QStyleOptionViewItem& option, const QModelIndex& index) override;

private:
    mutable int m_TagX, m_TagY;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_LIST_DELEGATE_H
