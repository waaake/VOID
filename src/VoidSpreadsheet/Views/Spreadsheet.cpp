// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QHeaderView>

/* Internal */
#include "Spreadsheet.h"
#include "VoidSpreadsheet/Delegates/ColorBoxDelegate.h"
#include "VoidSpreadsheet/Delegates/StatusDelegate.h"

VOID_NAMESPACE_OPEN

SpreadsheetTable::SpreadsheetTable(QWidget* parent)
    : QTableView(parent)
{
    Setup();
}

SpreadsheetTable::~SpreadsheetTable()
{
    m_Model->deleteLater();
    delete m_Model;
    m_Model = nullptr;

    m_Proxy->deleteLater();
    delete m_Proxy;
    m_Proxy = nullptr;
}

void SpreadsheetTable::ResetSequence(const SharedPlaybackSequence& sequence)
{
    m_Model->ResetSequence(sequence);
}

std::vector<SharedTrackItem> SpreadsheetTable::SelectedItems() const
{
    std::vector<SharedTrackItem> items;
    const QModelIndexList selected = selectedIndexes();
    items.resize(selected.size());

    std::transform(
        selected.begin(),
        selected.end(),
        items.begin(),
        [this](const QModelIndex& index) -> SharedTrackItem
        {
            return m_Model->Item(m_Proxy->mapToSource(index));
        }
    );

    return items;
}

void SpreadsheetTable::selectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{
    QTableView::selectionChanged(selected, deselected);
    emit itemSelectionChanged();
}

void SpreadsheetTable::Setup()
{
    m_Model = new SequenceItemsModel;
    m_Proxy = new SequenceItemsProxyModel;

    m_Proxy->setSourceModel(m_Model);
    setModel(m_Proxy);

    /// Attribs
    setAlternatingRowColors(true);
    setShowGrid(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSortingEnabled(true);
    verticalHeader()->setDefaultSectionSize(20);

    QHeaderView* header = horizontalHeader();
    header->setMinimumHeight(26);

    /// Column setup
    header->setStretchLastSection(true);

    /// Custom Delegates
    setItemDelegateForColumn(1, new LinkStatusDelegate(this));
    setItemDelegateForColumn(3, new ColorBoxDelegate(this));
}

VOID_NAMESPACE_CLOSE
