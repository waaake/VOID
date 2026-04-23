// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* Internal */
#include "Grader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

GradeOp::GradeOp()
{
    // std::string gain = "r_gain";
    m_RedGain = AddParam(Param("r_gain", "Red", 1.f, Param::TypeDesc::Float));
    m_GreenGain = AddParam(Param("g_gain", "Green", 1.f, Param::TypeDesc::Float));
    m_BlueGain = AddParam(Param("b_gain", "Blue", 1.f, Param::TypeDesc::Float));
    // m_GreenGain = Add
}

bool GradeOp::Evaluate(ImageRow& row)
{
    if (row.channels < 3)
        return false;

    // Test
    const float r = m_RedGain->GetFloat();
    const float g = m_GreenGain->GetFloat();
    const float b = m_BlueGain->GetFloat();
    // const float r = 1.0;
    // const float g = 1.0;
    // const float b = 1.0;

    // auto r_param = GetParam("r_gain");
    // if (m_RedGain)
    // {
    //     VOID_LOG_INFO("R Gain Param: {0}", m_RedGain->value.GetFloat());
    //     // auto v = r_param->value.Get<float>();
    //     // if (v)
    //     //     VOID_LOG_INFO("R Gain Param: {0}", *v);
    // }

    for (std::size_t i = 0; i < row.width; ++i)
    {
        unsigned char* pixel = row.Pixel<unsigned char>(i);
        
        pixel[0] = std::clamp(static_cast<int>(pixel[0] * r), 0, 255);
        pixel[1] = std::clamp(static_cast<int>(pixel[1] * g), 0, 255);
        pixel[2] = std::clamp(static_cast<int>(pixel[2] * b), 0, 255);
    }

    return true;
}

VOID_NAMESPACE_CLOSE
