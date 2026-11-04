// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cmath>
#include <string>

/* Internal */
#include "Engine.h"

VOID_NAMESPACE_OPEN

VoidEngineBridge& VoidEngineBridge::Instance()
{
    static VoidEngineBridge instance;
    return instance;
}

std::string VoidEngineBridge::FramerateString() const
{
    if (std::fabs(m_Framerate - std::round(m_Framerate)) < 1e-9)
        return std::to_string(static_cast<int>(m_Framerate));

    std::string fstr = std::to_string(m_Framerate);
    fstr.erase(fstr.find_last_not_of('0') + 1, std::string::npos);

    return fstr;
}

VOID_NAMESPACE_CLOSE
