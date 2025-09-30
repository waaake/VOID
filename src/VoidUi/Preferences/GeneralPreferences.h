// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_GENERAL_PREFERENCES_H
#define _VOID_GENERAL_PREFERENCES_H

/* Qt */
#include <QComboBox>
#include <QLabel>
#include <QLayout>

/* Internal */
#include "Definition.h"
#include "Preference.h"

VOID_NAMESPACE_OPEN

class GeneralPreferences : public BasicPreference
{
public:
    GeneralPreferences(QWidget* parent = nullptr);
    ~GeneralPreferences();

    void Reset() override;
    void Save() override;

private: /* Members */
    /* Main Layout */
    QGridLayout* m_Layout;

    /* Undo */
    QLabel* m_UndoDescription;
    QLabel* m_UndoLabel;
    QComboBox* m_UndoBox;

    /* ColorTheme */
    QLabel* m_ColorStyleDescription;
    QLabel* m_ColorStyleLabel;
    QComboBox* m_ColorStyleBox;

private: /* Methods */
    /**
     * Build UI layout
     */
    void Build();

    /**
     * Setup values from previous preferences
     */
    void Setup();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_GENERAL_PREFERENCES_H
