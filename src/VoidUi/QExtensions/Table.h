// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _Q_EXT_TABLE_H
#define _Q_EXT_TABLE_H

/* STD */
#include <vector>

/* Qt */
#include <QTableWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class FilterTable : public QTableWidget
{
    Q_OBJECT
public:
    FilterTable(const std::vector<QString>& filters, QWidget* parent = nullptr);

signals:
    void filterChanged(int);
};

VOID_NAMESPACE_CLOSE

#endif // _Q_EXT_TABLE_H