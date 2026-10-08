// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SPREADSHEET_WIDGET_H
#define _SPREADSHEET_WIDGET_H

/* Qt */
#include <QWidget>
#include <QLayout>

/* Internal */
#include "Definition.h"
#include "VoidSpreadsheet/PushButton.h"
#include "VoidSpreadsheet/Views/Spreadsheet.h"

VOID_NAMESPACE_OPEN

class SpreadsheetWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SpreadsheetWidget(QWidget* parent = nullptr);
    virtual ~SpreadsheetWidget();

    inline QSize sizeHint() const override { return QSize(300, 300); }

signals:
    void updateReferenceMedia(const SharedMediaClip&);

protected:
    QVBoxLayout* m_Layout;

    MediaDropButton* m_MatchMediaBtn;
    MediaDropButton* m_SetRefMediaBtn;

    SpreadsheetTable* m_Sheet;

private:
    void Build();
    void Setup();
    void SetReferenceMedia(const std::vector<SharedMediaClip>& media);
};

VOID_NAMESPACE_CLOSE

#endif // _SPREADSHEET_WIDGET_H
