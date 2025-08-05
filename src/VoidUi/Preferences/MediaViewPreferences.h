// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_VIEW_PREFERENCES_H
#define _VOID_MEDIA_VIEW_PREFERENCES_H

/* Qt */
#include <QComboBox>
#include <QLabel>
#include <QLayout>

/* Internal */
#include "Definition.h"
#include "Preference.h"

VOID_NAMESPACE_OPEN

class MediaViewPreferences : public BasicPreference
{
public:
    MediaViewPreferences(QWidget* parent = nullptr);
    ~MediaViewPreferences();

    void Reset() override;
    void Save() override;

private: /* Members */
    /* Main Layout */
    QGridLayout* m_Layout;

    /* View */
    QLabel* m_ViewDescription;
    QLabel* m_ViewLabel;
    QComboBox* m_ViewBox;

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

#endif // _VOID_MEDIA_VIEW_PREFERENCES_H
