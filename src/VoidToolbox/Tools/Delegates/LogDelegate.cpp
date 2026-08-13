// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPainter>

/* Internal */
#include "LogDelegate.h"
#include "VoidObjects/Core/TaskLog.h"

VOID_NAMESPACE_OPEN

ColoredLogDelegate::ColoredLogDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

void ColoredLogDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    painter->save();

    const TaskLog::Level level = static_cast<TaskLog::Level>(
            index.data(static_cast<int>(TaskLogModel::Roles::LevelRole)).toInt()
        );

    switch (level)
    {
        case TaskLog::Level::InfoLog:
            painter->setPen(QPen(QColor(140, 230, 50)));
            break;
        case TaskLog::Level::WarningLog:
            painter->setPen(QPen(QColor(230, 180, 50)));
            break;
        case TaskLog::Level::ErrorLog:
            painter->setPen(QPen(QColor(210, 0, 0)));
            break;
    }

    painter->drawText(option.rect, index.data(Qt::DisplayRole).toString());
    painter->restore();
}

VOID_NAMESPACE_CLOSE
