// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Internal */
#include "Brighter.h"

VOID_NAMESPACE_OPEN

BrightenOp::BrightenOp()
{
    m_Gain = AddParam(Param("gain", "Gain", 1.f, Param::TypeDesc::Float));
}

bool BrightenOp::Evaluate(ImageRow& row)
{
    // float gain = m_Gain->GetFloat();

    for (std::size_t i = 0; i < row.width; ++i)
    {
        unsigned char* pixel = row.Pixel<unsigned char>(i);
        for (std::size_t channel = 0; channel < 3; ++channel)
            pixel[channel] = std::min(255, int(pixel[channel] * m_Gain->GetFloat()));
    }

    return true;
}

VOID_NAMESPACE_CLOSE
