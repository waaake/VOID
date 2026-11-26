// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Tooltip.h"

VOID_NAMESPACE_OPEN

// std::string ToolTipString(const std::string& name, const std::string& description)
// {
//     if (description.empty())
//     {
//         constexpr std::string_view boldStart = "<b>";
//         constexpr std::string_view boldEnd = "</b>";
//         std::string result;
//         result.reserve(boldStart.size() + name.size() + boldEnd.size());

//         result.append(boldStart);
//         result.append(name);
//         result.append(boldEnd);

//         return result;
//     }

//     constexpr std::string_view boldStart = "<b>";
//     constexpr std::string_view boldBreak = "</b><br>";

//     std::string result;
//     result.reserve(boldStart.size() + name.size() + boldBreak.size() + description.size());
//     result.append(boldStart);
//     result.append(name);
//     result.append(boldBreak);
//     result.append(description);

//     return result;
// }

QString ToolTipString(const QString& name, const QString& description)
{
    QString s;
    if (description.isEmpty())
    {
        s.reserve(name.size() + 9);

        s += "<b>";
        s += name;
        s += "</b>";
        return s;
    }

    s.reserve(name.size() + description.size() + 13);

    s += "<b>";
    s += name;
    s += "</b><br>";
    s += description;

    return s;
}

VOID_NAMESPACE_CLOSE
