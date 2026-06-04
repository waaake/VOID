// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _TASK_LOG_H
#define _TASK_LOG_H

/* STD */
#include <vector>

/* Qt */
#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

struct VOID_API TaskLog
{
    enum class Level
    {
        DebugLog,
        InfoLog,
        WarningLog,
        ErrorLog,
        CriticalLog
    };

    QString text;
    Level level;

    TaskLog(const QString& text, const TaskLog::Level& level) : text(text), level(level) {}

    QString LogText() const
    {
        QString f("[%1] %2");
        switch (level)
        {
            case Level::DebugLog: return f.arg("DEBUG").arg(text);
            case Level::InfoLog: return f.arg("INFO").arg(text);
            case Level::WarningLog: return f.arg("WARNING").arg(text);
            case Level::ErrorLog: return f.arg("ERROR").arg(text);
            case Level::CriticalLog: return f.arg("CRITICAL").arg(text);
        }

        return text;
    }
};

class VOID_API TaskLogModel : public QAbstractItemModel
{
public:
    enum class Roles
    {
        LevelRole = Qt::UserRole + 1009
    };

public:
    TaskLogModel(QObject* parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    void AddLog(const TaskLog& log);
    void AddLog(const QString& text, const TaskLog::Level& level);
    void Clear();

private:
    std::vector<TaskLog> m_Logs;
};

class VOID_API TaskLogProxyModel : public QSortFilterProxyModel
{
public:
    TaskLogProxyModel(QObject* parent = nullptr);
    void SetLogLevel(const TaskLog::Level& level);
    void SetLogLevel(int level);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private: /* Members */
    int m_Level;
};

VOID_NAMESPACE_CLOSE

#endif // _TASK_LOG_H
