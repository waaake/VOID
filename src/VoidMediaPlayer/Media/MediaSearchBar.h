// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_SEARCH_BAR_H
#define _VOID_MEDIA_SEARCH_BAR_H

/* Qt */
#include <QAction>
#include <QLineEdit>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class MediaSearchBar : public QLineEdit
{
    Q_OBJECT
public:
    explicit MediaSearchBar(QWidget* parent = nullptr);
    ~MediaSearchBar();

signals:
    /* Signal for when something is just typed */
    void typed(const std::string&);
    /* When text is fully typed and return key was pressed */
    void searched(const std::string&);

private: /* Members */
    QAction* m_ClearAction;

private: /* Methods */
    /* Setup the UI */
    void Setup();

    /* Connect Signals */
    void Connect();

    /* When text is being typed on the Searchbar */
    void TextChanged(const QString& text);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_SEARCH_BAR_H
