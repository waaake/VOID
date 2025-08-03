// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PLAYER_PREFERENCES_H
#define _VOID_PLAYER_PREFERENCES_H

/* Qt */
#include <QComboBox>
// #include <QWidget>
#include <QLabel>
#include <QLayout>

/* Internal */
#include "Definition.h"
#include "Preference.h"

VOID_NAMESPACE_OPEN

class PlayerPreferences : public BasicPreference
{
public:
    PlayerPreferences(QWidget* parent = nullptr);
    ~PlayerPreferences();

    void Reset() override;
    void Save() override;

private: /* Members */
    /* Main Layout */
    QGridLayout* m_Layout;

    /* Missing Frames */
    QLabel* m_MissingFramesDescription;
    QLabel* m_MissingFramesLabel;
    QComboBox* m_MissingFramesBox;

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

#endif // _VOID_PLAYER_PREFERENCES_H