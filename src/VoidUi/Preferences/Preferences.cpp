// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Preferences.h"

VOID_NAMESPACE_OPEN

VoidPreferences::VoidPreferences(QObject* parent)
    : QObject(parent)
{
}

VoidPreferences& VoidPreferences::Instance()
{
    static VoidPreferences instance;
    return instance;
}

VoidPreferences::~VoidPreferences()
{
}

VOID_NAMESPACE_CLOSE
