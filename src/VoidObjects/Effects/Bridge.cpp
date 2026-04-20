// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Bridge.h"
#include "FormatForge.h"

VOID_NAMESPACE_OPEN

EffectsBridge& EffectsBridge::Instance()
{
    static EffectsBridge instance;
    return instance;
}

Effect* EffectsBridge::CreateEffect(const std::string& type)
{
    if (std::unique_ptr<ImageOp> creator = Forge::Instance().GetImageOp(type))
    {
        /**
         * We want to transfer the ownership of the created operator to the effect
         * such that it will be it's new parent and will decide when the creator needs to be deleted
         */
        return new Effect(creator.release(), EffectName(type));
    }
    return nullptr;
}

std::string EffectsBridge::EffectName(const std::string& type)
{
    std::string name = type;
    
    auto it = m_EffectCount.find(type);
    std::size_t count = it == m_EffectCount.end() ? 0 : it->second;
    count++;

    m_EffectCount[type] = count;
    name.append(std::to_string(count));

    return name;
}

VOID_NAMESPACE_CLOSE
