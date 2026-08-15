// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_SYNTAX_HIGHLIGHTER_H
#define _VOID_SYNTAX_HIGHLIGHTER_H

/* STD */
#include <vector>

/* Qt */
#include <QRegularExpression>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/**
 * Structures a Highlight rule
 * - Holds the possible regular expressions which will be checked against
 *   the text entered in the console
 * 
 * - The Text char format holding the formatting for the text in case
 *   a match occurs in the text
 */
struct HighlightRule
{
    QRegularExpression pattern;
    QTextCharFormat format;

    HighlightRule(const QRegularExpression& p, const QTextCharFormat& f)
        : pattern(p), format(f)
    {
    }
};

/**
 * Defines the Syntax Highlighting rules for python syntax
 */
class PySyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    explicit PySyntaxHighlighter(QTextDocument* document);

protected:
    void highlightBlock(const QString& text) override;

private: /* Members */
    std::vector<HighlightRule> m_Rules;

private: /* Methods */
    void SetupRules();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_SYNTAX_HIGHLIGHTER_H
