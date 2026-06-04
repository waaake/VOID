// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QHeaderView>

/* Internal */
#include "Table.h"

VOID_NAMESPACE_OPEN

FilterTable::FilterTable(const std::vector<QString>& filters, QWidget* parent)
    : QTableWidget(parent)
{
    setColumnCount(static_cast<int>(filters.size()));
    setRowCount(1);

    resizeColumnsToContents();

    int count = 0;
    int width = 0;

    QFontMetrics fm(font());

    for (const auto& filter : filters)
    {
        QTableWidgetItem* item = new QTableWidgetItem(filter);
        item->setTextAlignment(Qt::AlignCenter);
        setItem(0, count, item);

        int textWidth = fm.horizontalAdvance(filter);
        setColumnWidth(count, textWidth + 12);

        width += (textWidth + 12);
        count++;
    }

    connect(this, &QTableWidget::itemClicked, this, [this](QTableWidgetItem* item) -> void
    {
        emit filterChanged(item->column());
    });

    // Fixed size based on the filters
    setFixedHeight(fm.height() + 12);
    setFixedWidth(width + 6);

    // No Headers
    verticalHeader()->setVisible(false);
    horizontalHeader()->setVisible(false);
    horizontalHeader()->setStretchLastSection(true);

    setSelectionMode(QTableView::SelectionMode::SingleSelection);
    // Always the first is selected
    setItemSelected(item(0, 0), true);
}

VOID_NAMESPACE_CLOSE
