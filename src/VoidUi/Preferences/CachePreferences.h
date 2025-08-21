// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_CACHE_PREFERENCES_H
#define _VOID_CACHE_PREFERENCES_H

/* Qt */
#include <QLabel>
#include <QLayout>
#include <QSpinBox>

/* Internal */
#include "Definition.h"
#include "Preference.h"

VOID_NAMESPACE_OPEN

class CachePreferences : public BasicPreference
{
public:
    CachePreferences(QWidget* parent = nullptr);
    ~CachePreferences();

    void Reset() override;
    void Save() override;

private: /* Members */
    /* Main Layout */
    QGridLayout* m_Layout;

    /* Memory */
    QLabel* m_CacheDescription;
    QLabel* m_CacheLabel;
    QSpinBox* m_CacheBox;

    /* Threads */
    QLabel* m_ThreadsDescription;
    QLabel* m_ThreadsLabel;
    QSpinBox* m_ThreadsBox;

private: /* Methods */
    /**
     * Build UI layout
     */
    void Build();

    /**
     * Setup values from previous preferences
     */
    void Setup();

    size_t TotalMemory();
    unsigned int ProcessorCount();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_CACHE_PREFERENCES_H
