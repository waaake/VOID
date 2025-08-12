// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PY_SCRIPT_EDITOR_H
#define _VOID_PY_SCRIPT_EDITOR_H

/* Qt */
#include <QLayout>
#include <QSplitter>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QWidget>

/* Internal */
#include "Definition.h"
#include "ScriptConsole.h"
#include "SyntaxHighlighter.h"
#include "VoidCore/PyExecutor.h"

VOID_NAMESPACE_OPEN

class PyScriptEditor : public QWidget
{
    Q_OBJECT

public:
    PyScriptEditor(QWidget* parent = nullptr);
    ~PyScriptEditor();

private: /* Members */
    QVBoxLayout* m_Layout;
    QHBoxLayout* m_ButtonLayout;

    QSplitter* m_InternalSplitter;
    QPlainTextEdit* m_OutputConsole;
    InputScriptConsole* m_InputConsole;

    PySyntaxHighlighter* m_SyntaxHighlighter;

    QPushButton* m_ExecAllButton;
    QPushButton* m_ExecSelectionButton;

    QPushButton* m_SaveScriptButton;
    QPushButton* m_LoadScriptButton;

    QPushButton* m_ClearOutputButton;

    /* Core Python Executor */
    PyExecutor* m_Executor;

private: /* Methods */
    void Build();
    void Connect();

    void Setup();

    void ExecuteAll();
    void ExecuteSelection();

    void SaveScript();
    void LoadScript();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PY_SCRIPT_EDITOR_H
