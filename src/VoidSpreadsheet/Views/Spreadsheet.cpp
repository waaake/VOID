// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Spreadsheet.h"

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
    // for (const QModelIndex& index : )
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

void SpreadsheetTable::Setup()
{
    m_Model = new SequenceItemsModel;
    m_Proxy = new SequenceItemsProxyModel;

    m_Proxy->setSourceModel(m_Model);
    setModel(m_Proxy);

    setAlternatingRowColors(true);
    setShowGrid(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // connect(this, &QTableView::clicked, this, [this](const QModelIndex& index) -> void
    // {
    //     emit itemSelected(m_Model->Item(m_Proxy->mapToSource(index)));
    // });
}

VOID_NAMESPACE_CLOSE
