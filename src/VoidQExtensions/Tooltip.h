// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_Q_EXT_TOOLTIP_H
#define _VOID_Q_EXT_TOOLTIP_H

// /* STD */
// #include <string>

/* Qt */
#include <QString>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

// std::string VOID_API ToolTipString(const std::string& name, const std::string& description);
QString VOID_API ToolTipString(const QString& name, const QString& description);

VOID_NAMESPACE_CLOSE

#endif // _VOID_Q_EXT_TOOLTIP_H
