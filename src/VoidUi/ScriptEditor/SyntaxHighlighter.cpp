// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include "SyntaxHighlighter.h"

VOID_NAMESPACE_OPEN

PySyntaxHighlighter::PySyntaxHighlighter(QTextDocument* document)
    : QSyntaxHighlighter(document)
{
    SetupRules();
}

void PySyntaxHighlighter::highlightBlock(const QString& text)
{
    for (const HighlightRule& rule: m_Rules)
    {
        QRegularExpressionMatchIterator it = rule.pattern.globalMatch(text);

        while (it.hasNext())
        {
            QRegularExpressionMatch match = it.next();

            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

void PySyntaxHighlighter::SetupRules()
{
    /**
     * Python Keywords
     */
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(180, 110, 210));      // Pale Pink;
    keywordFormat.setFontWeight(QFont::Bold);

    QStringList keywords = {
        "def", "class", "if", "else", "elif", "while", "for", "in",
        "try", "except", "finally", "return", "import", "from", "as",
        "pass", "break", "continue", "with", "lambda", "yield", "global",
        "nonlocal", "assert", "del", "raise", "is", "not", "and", "or"
    };

    m_Rules.reserve(keywords.size() + 4);

    for (const QString& keyword : keywords)
    {
        QRegularExpression pattern(QString("\\b%1\\b").arg(keyword));

        m_Rules.emplace_back(pattern, keywordFormat);
    }

    /**
     * Comments starting with #
     */
    QRegularExpression commentPattern("#[^\n]*");
    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(130, 130, 130));      // Grey;
    commentFormat.setFontItalic(true);
    commentFormat.setFontWeight(QFont::Bold);

    m_Rules.emplace_back(commentPattern, commentFormat);

    /**
     * Self
     */
    QRegularExpression selfPattern("\\bself\\b");
    QTextCharFormat selfFormat;
    selfFormat.setForeground(QColor(140, 210, 220));        // Blue;

    m_Rules.emplace_back(selfPattern, selfFormat);

    /**
     * Quoted strings
     * " "
     * ' '
     */
    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(160, 210, 90));        // Green;

    /* "" */
    m_Rules.emplace_back(QRegularExpression("\"[^\"]*\""), stringFormat);
    /* '' */
    m_Rules.emplace_back(QRegularExpression("'[^']*'"), stringFormat);
}

VOID_NAMESPACE_CLOSE
