// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SPREADSHEET_WIDGET_H
#define _SPREADSHEET_WIDGET_H

/* Qt */
#include <QWidget>
#include <QLayout>
#include <QPushButton>

/* Internal */
#include "Definition.h"
#include "VoidSpreadsheet/Views/Spreadsheet.h"

VOID_NAMESPACE_OPEN

class SpreadsheetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpreadsheetWidget(QWidget* parent = nullptr);
    virtual ~SpreadsheetWidget();

    inline QSize sizeHint() const override { return QSize(300, 300); }

protected:
    QVBoxLayout* m_Layout;

    QPushButton* m_MatchMediaBtn;
    QPushButton* m_SetRefMediaBtn;

    SpreadsheetTable* m_Sheet;

private:
    void Build();
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _SPREADSHEET_WIDGET_H
