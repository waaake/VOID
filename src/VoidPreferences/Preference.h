// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_BASE_PREFERENCE_H
#define _VOID_BASE_PREFERENCE_H

/* Qt */
#include <QWidget>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class BasicPreference : public QWidget
{
public:
    BasicPreference(QWidget* parent = nullptr) : QWidget(parent) {};

    virtual ~BasicPreference() {};
    /**
     * Reload the setting information back from the global settings
     */
    virtual void Reset() = 0;

    /**
     * Saves the current settings on the global settings
     */
    virtual void Save() = 0;

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_BASE_PREFERENCE_H