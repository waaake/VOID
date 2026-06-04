// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _LOG_WINDOW_H
#define _LOG_WINDOW_H

/* Qt */
#include <QLayout>
#include <QListView>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Core/TaskLog.h"
#include "VoidUi/QExtensions/Dialog.h"
#include "VoidUi/QExtensions/Table.h"

VOID_NAMESPACE_OPEN

class LoggerWindow : public TranslucentDialog
{
public:
    LoggerWindow(TaskLogModel* model, QWidget* parent = nullptr);
    ~LoggerWindow();

    inline QSize sizeHint() const override { return QSize(1024, 768); }

private: /* Members */
    QVBoxLayout* m_Layout;
    QListView* m_LogView;
    FilterTable* m_Filter;
    TaskLogProxyModel* m_Proxy;

private: /* Methods */
    void Build();
    void Connect();
};

VOID_NAMESPACE_CLOSE

#endif // _LOG_WINDOW_H
