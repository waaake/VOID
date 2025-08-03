// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Tooltip.h"

VOID_NAMESPACE_OPEN

std::string ToolTipString(const std::string& name, const std::string& description)
{
    /* The HTML Elements which will be used for ToolTip Formatting */
    std::string boldStart = "<b>";
    std::string boldBreak = "</b><br>";

    std::string result;
    /* Reserve max size upfront */
    result.reserve(boldStart.size() + name.size() + boldBreak.size() + description.size());
    /* Add to result */
    result.append(boldStart);
    result.append(name);
    result.append(boldBreak);
    result.append(description);

    /**
     * The resulting string is of the format
     * <b>Name</b><br>description
     * This format can be resolved by the QToolTip to make the name bolder and the description in the next line
     */
    return result;
}

VOID_NAMESPACE_CLOSE
