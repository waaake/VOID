// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SPREADSHEET_VIEW_H
#define _SPREADSHEET_VIEW_H

/* Qt */
#include <QTableView>

/* Internal */
#include "Definition.h"
#include "VoidSpreadsheet/Models/Spreadsheet.h"

VOID_NAMESPACE_OPEN

class SpreadsheetTable : public QTableView
{
    Q_OBJECT
public:
    SpreadsheetTable(QWidget* parent = nullptr);
    ~SpreadsheetTable();

    void ResetSequence(const SharedPlaybackSequence& sequence);
    std::vector<SharedTrackItem> SelectedItems() const;

signals:
    void itemSelected(const SharedTrackItem&);

private:
    SequenceItemsModel* m_Model;
    SequenceItemsProxyModel* m_Proxy;

private:
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // SPREADSHEET_VIEW_H
