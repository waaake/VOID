// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Tokens.h"
#include "VoidCore/VoidTools.h"

VOID_NAMESPACE_OPEN

void ElementTokens::ParseVersion()
{
    // Assuming the version has a single 'v' and is like 'v001' or 'V01' or something similar
    std::string num = version.substr(1);
    if (Tools::isnum(num))
        vnum = std::stoi(num);
}

VOID_NAMESPACE_CLOSE
