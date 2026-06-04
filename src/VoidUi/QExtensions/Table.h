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

/**
 * @brief A table widget acting like a quick filter view
 * Takes in a vector of items to be displayed in the filter
 * 
 * @param filters Names of the filters to be displayed on the view.
 * @param selected Index of the default selection. The item at this index is selected/highlighted by default.
 * @param parent Parent widget to this filter, if any.
 */
class FilterTable : public QTableWidget
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Filter view with provided options
     * 
     * @param filters Names of the filters to be displayed on the view.
     * @param selected Index of the default selection. The item at this index is selected/highlighted by default.
     * @param parent Parent widget to this filter, if any.
     */
    FilterTable(const std::vector<QString>& filters, int selected = 0, QWidget* parent = nullptr);

signals:
    void filterChanged(int);
};

VOID_NAMESPACE_CLOSE

#endif // _Q_EXT_TABLE_H