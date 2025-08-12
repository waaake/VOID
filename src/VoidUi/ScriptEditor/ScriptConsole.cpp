// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QKeyEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QTextBlock>

/* Internal */
#include "ScriptConsole.h"

VOID_NAMESPACE_OPEN

/* Line Number Area {{{ */
LineNumberArea::LineNumberArea(InputScriptConsole* console)
    : QWidget(console)
    , m_Console(console)
{
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(m_Console->LineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent* event)
{
    m_Console->LineNumberPaintEvent(event);
}

/* }}} */

InputScriptConsole::InputScriptConsole(QWidget* parent)
    : QPlainTextEdit(parent)
{
    m_LineNumberArea = new LineNumberArea(this);

    /* Signals */
    Connect();
    UpdateLineNumberAreaWidth(0);
}

InputScriptConsole::~InputScriptConsole()
{
}

std::string InputScriptConsole::SelectedText() const
{
    const QTextCursor& c = textCursor();
    if (!c.hasSelection())
        return "";

    QTextCursor lineCursor = textCursor();
    lineCursor.setPosition(c.selectionStart());
    int start = lineCursor.blockNumber();

    lineCursor.setPosition(c.selectionEnd());
    int end = lineCursor.blockNumber();

    QStringList lines;
    for (int i = start; i <= end; ++i)
    {
        lines << document()->findBlockByNumber(i).text();
    }

    return lines.join("\n").toStdString();
}

void InputScriptConsole::resizeEvent(QResizeEvent* event)
{
    QPlainTextEdit::resizeEvent(event);

    QRect crect = contentsRect();
    m_LineNumberArea->setGeometry(QRect(crect.left(), crect.top(), LineNumberAreaWidth(), crect.height()));
}

void InputScriptConsole::keyPressEvent(QKeyEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier && event->key() == Qt::Key_Return)
    {
        if (textCursor().hasSelection())
            emit executeSelected();
        else
            emit execute();
        return;
    }

    if (event->key() == Qt::Key_Tab)
        Indent();
    else
        QPlainTextEdit::keyPressEvent(event);
}

void InputScriptConsole::Indent()
{
    QTextCursor c = textCursor();
    int column = c.positionInBlock();

    int spaces = 4 - (column % 4);
    c.insertText(QString(spaces, ' '));
    setTextCursor(c);
}

void InputScriptConsole::Connect()
{
    connect(this, &QPlainTextEdit::blockCountChanged, this, &InputScriptConsole::UpdateLineNumberAreaWidth);
    connect(this, &QPlainTextEdit::updateRequest, this, &InputScriptConsole::UpdateLineNumberArea);
    connect(this, &QPlainTextEdit::cursorPositionChanged, this, &InputScriptConsole::HighlightCurrentLine);
}

void InputScriptConsole::HighlightCurrentLine()
{
    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(palette().color(QPalette::Base).lighter(200));
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    setExtraSelections({selection});
    update();
}

int InputScriptConsole::LineNumberAreaWidth()
{
    int digits = 1;
    int max = std::max(digits, blockCount());

    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    return 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
}

void InputScriptConsole::UpdateLineNumberArea(const QRect& rect, int dy)
{
    if (dy)
        m_LineNumberArea->scroll(0, dy);
    else
        m_LineNumberArea->update(0, rect.y(), m_LineNumberArea->width(), rect.height());
    
    if (rect.contains(viewport()->rect()))
        UpdateLineNumberAreaWidth(0);
}

void InputScriptConsole::UpdateLineNumberAreaWidth(int)
{
    setViewportMargins(LineNumberAreaWidth(), 0, 0, 0);
}

void InputScriptConsole::LineNumberPaintEvent(QPaintEvent* event)
{
    QPainter painter(m_LineNumberArea);
    painter.fillRect(event->rect(), palette().color(QPalette::Base));

    QTextBlock block = firstVisibleBlock();
    int blockNum = block.blockNumber();

    int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + static_cast<int>(blockBoundingRect(block).height());

    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            /* Draw Line Number */
            QString number = QString::number(blockNum + 1);

            painter.setPen(palette().color(QPalette::Text).darker(150));
            painter.drawText(0, top, m_LineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        /* Switch the top-bottom */
        top = bottom;
        /* This needs to be queried again as the line could be warped into multi lines */
        bottom = top + static_cast<int>(blockBoundingRect(block).height());

        ++blockNum;
    }
}

VOID_NAMESPACE_CLOSE
