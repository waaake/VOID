// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Tooltip.h"

VOID_NAMESPACE_OPEN

std::string ToolTipString(const std::string& name, const std::string& description)
{
    if (description.empty())
    {
        std::string boldStart = "<b>";
        std::string boldEnd = "</b>";
        std::string result;
        result.reserve(boldStart.size() + name.size() + boldEnd.size());

        result.append(boldStart);
        result.append(name);
        result.append(boldEnd);

        return result;
    }

    std::string boldStart = "<b>";
    std::string boldBreak = "</b><br>";

    std::string result;
    result.reserve(boldStart.size() + name.size() + boldBreak.size() + description.size());
    result.append(boldStart);
    result.append(name);
    result.append(boldBreak);
    result.append(description);

    return result;
}

VOID_NAMESPACE_CLOSE
