// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>
#include <cmath>

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

    // VOID_LOG_INFO("R: {0}, G: {1}, B: {2}", r, g, b);

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

Grade2::Grade2()
{
    m_Blackpoint = AddParam(Param("blackpoint", "Blackpoint", 0.f, Param::TypeDesc::Float));
    m_Whitepoint = AddParam(Param("whitepoint", "Whitepoint", 1.f, Param::TypeDesc::Float));
    m_Lift = AddParam(Param("lift", "Lift", 0.f, Param::TypeDesc::Float));
    m_Gain = AddParam(Param("gain", "Gain", 1.f, Param::TypeDesc::Float));
    m_Multiply = AddParam(Param("multiply", "Multiply", 1.f, Param::TypeDesc::Float));
    m_Offset = AddParam(Param("offset", "Offset", 0.f, Param::TypeDesc::Float));
    m_Gamma = AddParam(Param("gamma", "Gamma", 1.f, Param::TypeDesc::Float));
}

bool Grade2::Evaluate(ImageRow& row)
{
    const float blackpoint = m_Blackpoint->GetFloat();
    const float whitepoint = m_Whitepoint->GetFloat();
    const float lift = m_Lift->GetFloat();
    const float gain = m_Gain->GetFloat();
    const float multiply = m_Multiply->GetFloat();
    const float offset = m_Offset->GetFloat();
    const float gamma = m_Gamma->GetFloat();

    // Based on formula from https://www.chrisbturner.com/blog/nukes-grade-node-demystified
    // pow((((value - blackpoint) / (whitepoint - blackpoint) * ((gain * multiply) - lift)) + lift) + offset, (1 / gamma))
    for (std::size_t i = 0; i < row.width; ++i)
    {
        unsigned char* pixel = row.Pixel<unsigned char>(i);
        for (std::size_t channel = 0; channel < row.channels; ++channel)
        {
            float value = pixel[channel] / 255.f;

            // value = (value - blackpoint) / (whitepoint - blackpoint);
            // value = (value + lift) * gain;
            // value = value * multiply + offset;
            // value = std::pow(value, gamma);
            value = std::pow((((value - blackpoint) / (whitepoint - blackpoint) * ((gain * multiply) - lift)) + lift) + offset, (1 / gamma));

            // pixel[channel] = static_cast<unsigned char>(std::clamp<int>())
            // value = std::max(0.f, )
            // value = std::clamp<float>(value, 0.f, 1.f);
            pixel[channel] = static_cast<unsigned char>(std::clamp<float>(value, 0.f, 1.f) * 255.f);
        }
    }

    return true;
}

VOID_NAMESPACE_CLOSE
