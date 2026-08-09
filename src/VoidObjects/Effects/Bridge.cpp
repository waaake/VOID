// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <sstream>

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

Effect* EffectsBridge::CreateEffect(const std::string& type, v_frame_t in, v_frame_t out)
{
    if (std::unique_ptr<ImageOp> creator = Forge::Instance().GetImageOp(type))
    {
        /**
         * We want to transfer the ownership of the created operator to the effect
         * such that it will be it's new parent and will decide when the creator needs to be deleted
         */
        return new Effect(creator.release(), EffectName(type), in, out);
    }
    return nullptr;
}

Effect* EffectsBridge::Copy(const Effect* effect)
{
    Effect* copied = new Effect(Forge::Instance().GetImageOp(effect->Type()).release(), "copy");

    // Copy internals and values
    std::ostringstream out(std::ios::binary);
    effect->Serialize(out);
    std::istringstream in(out.str(), std::ios::binary);
    copied->Deserialize(in);

    // Finally set the next available name
    copied->SetName(EffectName(effect->Type()));
    return copied;
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
