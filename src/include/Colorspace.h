// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_COLORSPACE_H
#define _VOID_COLORSPACE_H

/* STD */
#include <string>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

/// @brief Defines the colorspace type for the media
enum class Colorspace
{
    Unknown,

    // Standard RGB
    SRGB,
    Rec709,
    Rec2020,
    AdobeRGB,

    // Linear RGB
    LinearSRGB,
    LinearRec709,

    // ACES
    ACES2065_1,
    ACEScg,

    // Log
    ARRI_LogC3,
    ARRI_LogC4,
    Sony_SLog3,
    Panasonic_VLog,
    Canon_Log3,

    // HDR
    ST2084_PQ,
    HLG,

    // Other custom
    Custom
};

/// @brief Describes the Transfer function for the image
/// Each pixel will have it's color values i.e the chromaticity
/// and then the light intensity in the pixel defines how the final color of the pixel appears
enum class TransferFunction
{
    Unknown,
    Linear,
    SRGB,
    Rec709,
    Gamma22,
    Gamma24,
    Log,
    PQ,
    HLG
};

/// @brief Defines the overall color related information for the image/media
/// Defines input colorspace, transfer function
/// name of the original colorspace, and custom, if overridden
/// stores whether the media has an icc profile etc.
struct MediaColor
{
    Colorspace colorspace = Colorspace::Unknown;
    TransferFunction transfer = TransferFunction::Unknown;

    /// @brief colorspace name from the source metadata, if found
    std::string name;
    /// @brief custom name when overriding the colorspace manually
    std::string custom;

    /// @brief if we have ICC profile embedded in the media metadata
    bool iccprofile = false;

    bool hdr = false;
    bool colorspaceMetadata = false;

    /// @brief if we have information about the color values
    bool colorPrimaries = false;

    bool operator==(const MediaColor& other) const
    { 
        if (colorspace == Colorspace::Custom)
            return other.colorspace == colorspace && other.custom == custom;

        return other.colorspace == colorspace && other.transfer == transfer && other.iccprofile == iccprofile;
    }
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_COLORSPACE_H
