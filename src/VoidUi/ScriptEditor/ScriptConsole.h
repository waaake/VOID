// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PY_SCRIPT_CONSOLE_H
#define _VOID_PY_SCRIPT_CONSOLE_H

/* Qt */
#include <QPlainTextEdit>
#include <QWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/* Forward Decl */
class InputScriptConsole;

class LineNumberArea : public QWidget
{
public:
    explicit LineNumberArea(InputScriptConsole* console);
    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    InputScriptConsole* m_Console;
};

class InputScriptConsole : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit InputScriptConsole(QWidget* parent = nullptr);
    ~InputScriptConsole();

    std::string SelectedText() const;

signals:
    void execute();
    void executeSelected();

protected:
    void resizeEvent(QResizeEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private: /* Members */
    LineNumberArea* m_LineNumberArea;

private: /* Methods */
    void Connect();

    int LineNumberAreaWidth();
    void LineNumberPaintEvent(QPaintEvent* event);

    void UpdateLineNumberAreaWidth(int);
    void UpdateLineNumberArea(const QRect&, int);

    void HighlightCurrentLine();
    void Indent();

private:
    friend class LineNumberArea;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_PY_SCRIPT_CONSOLE_H
