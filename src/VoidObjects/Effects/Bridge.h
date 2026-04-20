// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _EFFECTS_BRIDGE_H
#define _EFFECTS_BRIDGE_H

/* STD */
#include <unordered_map>
// #include <vector>

/* Internal */
#include "Definition.h"
#include "Effects.h"

VOID_NAMESPACE_OPEN

class EffectsBridge
{
    EffectsBridge() = default;
public:
    static EffectsBridge& Instance();

    Effect* CreateEffect(const std::string& type);

private: /* Members */
    std::unordered_map<std::string, std::size_t> m_EffectCount;

private: /* Methods */
    std::string EffectName(const std::string& type);
};

#define _EffectsBridge EffectsBridge::Instance()

VOID_NAMESPACE_CLOSE

#endif // _EFFECTS_BRIDGE_H
