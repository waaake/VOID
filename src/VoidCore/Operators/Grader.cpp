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
    m_RedGain = AddParam("r_gain", "Red", 1.f, Param::TypeDesc::Float);
    m_GreenGain = AddParam("g_gain", "Green", 1.f, Param::TypeDesc::Float);
    m_BlueGain = AddParam("b_gain", "Blue", 1.f, Param::TypeDesc::Float);
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
    m_ChannelRed = AddParam("channel_red", "Red", true, Param::TypeDesc::Boolean);
    m_ChannelGreen = AddParam("channel_green", "Green", true, Param::TypeDesc::Boolean);
    m_ChannelBlue = AddParam("channel_blue", "Blue", true, Param::TypeDesc::Boolean);

    m_Blackpoint = AddParam(
        "blackpoint", "Blackpoint", "Defines input values that maps to pure black.",
        0.f, ParamRange(-1.f, 1.f, 0.01f), Param::TypeDesc::Float
    );
    m_Whitepoint = AddParam(
        "whitepoint", "Whitepoint", "Defines input values that maps to pure white.",
        1.f, ParamRange(1.f, 4.f, 0.01f), Param::TypeDesc::Float
    );
    m_Lift = AddParam(
        "lift", "Lift", "Adjusts the brightness of shadows.",
        0.f, ParamRange(-1.f, 1.f, 0.1f), Param::TypeDesc::Float
    );
    m_Gain = AddParam(
        "gain", "Gain", "Scales the intensity of highlights.",
        1.f, ParamRange(0.f, 4.f, 0.1f), Param::TypeDesc::Float
    );
    m_Multiply = AddParam(
        "multiply", "Multiply", "Scales the image brightness.",
        1.f, ParamRange(0.f, 4.f, 0.1f), Param::TypeDesc::Float
    );
    m_Offset = AddParam(
        "offset", "Offset", "Shifts brighness up or down across pixels.",
        0.f, ParamRange(-1.f, 1.f, 0.01f), Param::TypeDesc::Float
    );
    m_Gamma = AddParam(
        "gamma", "Gamma", "Controls midtones contrast with non-linear adjustment.",
        1.f, ParamRange(0.2f, 5.f, 0.1f), Param::TypeDesc::Float
    );
}

bool Grade2::Evaluate(ImageRow& row)
{
    if (row.channels < 3)
        return false;

    const float blackpoint = m_Blackpoint->GetFloat();
    const float whitepoint = m_Whitepoint->GetFloat();
    const float lift = m_Lift->GetFloat();
    const float gain = m_Gain->GetFloat();
    const float multiply = m_Multiply->GetFloat();
    const float offset = m_Offset->GetFloat();
    const float gamma = m_Gamma->GetFloat();

    const bool redchan = m_ChannelRed->GetBool();
    const bool greenchan = m_ChannelGreen->GetBool();
    const bool bluechan = m_ChannelBlue->GetBool();

    // Based on formula from https://www.chrisbturner.com/blog/nukes-grade-node-demystified
    // pow((((x - blackpoint) / (whitepoint - blackpoint) * ((gain * multiply) - lift)) + lift) + offset, (1 / gamma))
    for (std::size_t i = 0; i < row.width; ++i)
    {
        unsigned char* pixel = row.Pixel<unsigned char>(i);

        // Red
        if (redchan)
        {
            float red = pixel[0] / 255.f;
            red = std::pow((((red - blackpoint) / (whitepoint - blackpoint) * ((gain * multiply) - lift)) + lift) + offset, (1 / gamma));
            pixel[0] = static_cast<unsigned char>(std::clamp<float>(red, 0.f, 1.f) * 255.f);
        }

        // Green
        if (greenchan)
        {
            float green = pixel[1] / 255.f;
            green = std::pow((((green - blackpoint) / (whitepoint - blackpoint) * ((gain * multiply) - lift)) + lift) + offset, (1 / gamma));
            pixel[1] = static_cast<unsigned char>(std::clamp<float>(green, 0.f, 1.f) * 255.f);
        }

        // Blue
        if (bluechan)
        {
            float blue = pixel[2] / 255.f;
            blue = std::pow((((blue - blackpoint) / (whitepoint - blackpoint) * ((gain * multiply) - lift)) + lift) + offset, (1 / gamma));
            pixel[2] = static_cast<unsigned char>(std::clamp<float>(blue, 0.f, 1.f) * 255.f);
        }

        // Alpha
        if (row.channels > 3)
        {
            float alpha = pixel[3] / 255.f;
            alpha = std::pow((((alpha - blackpoint) / (whitepoint - blackpoint) * ((gain * multiply) - lift)) + lift) + offset, (1 / gamma));
            pixel[3] = static_cast<unsigned char>(std::clamp<float>(alpha, 0.f, 1.f) * 255.f);
        }
    }

    return true;
}

VOID_NAMESPACE_CLOSE
