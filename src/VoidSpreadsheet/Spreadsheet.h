// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _SPREADSHEET_H
#define _SPREADSHEET_H

/* Internal */
#include "Definition.h"
#include "SpreadsheetWidget.h"

VOID_NAMESPACE_OPEN

class SequencerContext;

class VOID_API Spreadsheet : public SpreadsheetWidget
{
public:
    explicit Spreadsheet(QWidget* parent = nullptr);
    ~Spreadsheet();

    void SetContext(SequencerContext* context);
    void SetSequence(const SharedPlaybackSequence& sequence);

private:
    SequencerContext* m_Context;

private:
    void Setup();
};

VOID_NAMESPACE_CLOSE

#endif // _SPREADSHEET_H
