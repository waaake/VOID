// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>

/* OpenImageIO */
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebufalgo.h>

/* Internal */
#include "OIIOReader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

// OIIOPixReader::OIIOPixReader(const std::string& path, v_frame_t framenumber)
//     : VoidPixReader(path, framenumber)
// {
// }

// OIIOPixReader::~OIIOPixReader()
// {
//     Clear();
// }

// void OIIOPixReader::ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image)
// {
//     std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(path);
//     if (!input)
//     {
//         VOID_LOG_INFO("Unable to load image. Path: {0}", path);
//         // Fetch the error from OpenImageIO
//         VOID_LOG_ERROR(OIIO::geterror());
//         return;
//     }

//     // Read image spec
//     const OIIO::ImageSpec spec = input->spec();
//     image->width = spec.width;
//     image->height = spec.height;
//     image->channels = spec.nchannels;
//     image->format = (image->channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA;

//     /// Underlying image specs
//     m_Image->width = spec.width;
//     m_Image->height = spec.height;
//     m_Image->channels = spec.nchannels;
//     m_Image->format = (m_Image->channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA;
//     m_Image->type = VOID_GL_FLOAT;

//     image->buffer.Resize(image->width * image->height * image->channels);
//     input->read_image(0, 0, 0, image->channels, OIIO::TypeDesc::UINT8, image->Writable());
//     input->close();
// }

// void OIIOPixReader::Clear()
// {
//     m_Image->Clear();
// }

// void OIIOPixReader::Read(const std::string& path, v_frame_t frame, FloatImage& image)
// {
//     std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(path);
//     if (!input)
//     {
//         VOID_LOG_INFO("Unable to load image. Path: {0}", path);
//         // Fetch the error from OpenImageIO
//         VOID_LOG_ERROR(OIIO::geterror());
//         return;
//     }

//     // Read image spec
//     const OIIO::ImageSpec spec = input->spec();
//     image->width = spec.width;
//     image->height = spec.height;
//     image->channels = spec.nchannels;
//     image->format = (image->channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA;
//     image->type = VOID_GL_FLOAT;

//     image->buffer.Resize(image->width * image->height * image->channels);

//     input->read_image(0, 0, 0, image->channels, OIIO::TypeDesc::FLOAT, image->Writable());
//     input->close();
// }

// void OIIOPixReader::Read()
// {
//     std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(m_Path);
//     if (!input)
//     {
//         VOID_LOG_INFO("Unable to load image. Path: {0}", m_Path);
//         // Fetch the error from OpenImageIO
//         VOID_LOG_ERROR(OIIO::geterror());
//         return;
//     }

//     // Read image spec
//     const OIIO::ImageSpec spec = input->spec();
//     m_Image->width = spec.width;
//     m_Image->height = spec.height;
//     m_Image->channels = spec.nchannels;
//     m_Image->format = (m_Image->channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA;
//     m_Image->type = VOID_GL_FLOAT;

//     m_Image->buffer.Resize(m_Image->width * m_Image->height * m_Image->channels);

//     input->read_image(0, 0, 0, m_Image->channels, OIIO::TypeDesc::FLOAT, m_Image->Writable());
//     input->close();
// }

// MediaColor OIIOPixReader::MediaColorInfo() const
// {
//     MediaColor color;
//     std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(m_Path);
//     if (!input)
//     {
//         VOID_LOG_INFO("Unable to load image. Path: {0}", m_Path);
//         // Fetch the error from OpenImageIO
//         VOID_LOG_ERROR(OIIO::geterror());
//         return color;
//     }

//     // Read image spec
//     const OIIO::ImageSpec spec = input->spec();
//     input->close();

//     std::string_view colorspace = spec.get_string_attribute("oiio:ColorSpace");
//     if (!colorspace.empty())
//     {
//         // VOID_LOG_INFO("Colorspace: {}", colorspace);
//         if (colorspace == "sRGB")
//             color.colorspace = Colorspace::SRGB;
//         else if (colorspace == "Rec709")
//             color.colorspace = Colorspace::Rec709;
//         else if (colorspace == "AdobeRGB")
//             color.colorspace = Colorspace::AdobeRGB;
//         else if (colorspace == "ACEScg" || colorspace == "lin_ap1_scene")
//             color.colorspace = Colorspace::ACEScg;
//         else if (colorspace == "lin_ap0_scene")
//             color.colorspace = Colorspace::ACES2065_1;
//         else if (colorspace == "lin_rec709" || colorspace == "lin_rec709_scene" || colorspace == "Linear")
//             color.colorspace = Colorspace::LinearRec709;
//         else
//             color.colorspace = Colorspace::Custom;
//     }

//     const OIIO::ParamValue* icc = spec.find_attribute("ICCProfile");
//     if (icc)
//         color.iccprofile = true;

//     const float gamma = spec.get_float_attribute("oiio:Gamma");
//     // VOID_LOG_INFO("Gamma: {}", gamma);
//     if (gamma > 0.f)
//     {
//         if (std::abs(gamma - 2.2f) < 0.01f)
//             color.transfer = TransferFunction::Gamma22;
//         else if (std::abs(gamma - 2.4f) < 0.01f)
//             color.transfer = TransferFunction::Gamma24;
//     }

//     return color;
// }

// const std::map<std::string, std::string> OIIOPixReader::Metadata() const
// {
//     std::map<std::string, std::string> m;

//     std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(m_Path);
//     if (!input)
//     {
//         VOID_LOG_INFO("Unable to load image. Path: {0}", m_Path);
//         /* Log the original error from OpenImageIO */
//         VOID_LOG_ERROR(OIIO::geterror());
//         return m;
//     }

//     const OIIO::ImageSpec spec = input->spec();

//     /* Basic Metadata */
//     m["filepath"] = m_Path;
//     m["width"] = std::to_string(spec.width);
//     m["height"] = std::to_string(spec.height);
//     m["channels"] = std::to_string(spec.nchannels);
//     m["colorspace"] = spec.get_string_attribute("oiio::ColorSpace");
//     m["bit_depth"] = std::to_string(spec.get_int_attribute("BitsPerSample", 8));
//     m["format"] = spec.format.c_str();

//     /* Additional */
//     for (const OIIO::ParamValue& attr : spec.extra_attribs)
//     {
//         if (attr.type() == OIIO::TypeDesc::STRING)
//             m[attr.name().c_str()] = *(const char**)attr.data();
//     }

//     return m;
// }

bool OIIOMediaReader::Read(const std::string& path, v_frame_t framenumber, UInt8Image& image)
{
    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(path);
    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", path);
        // Fetch the error from OpenImageIO
        VOID_LOG_ERROR(OIIO::geterror());
        return false;
    }

    // Read image spec
    const OIIO::ImageSpec spec = input->spec();
    image->width = spec.width;
    image->height = spec.height;
    image->channels = spec.nchannels;
    image->format = (image->channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA;

    image->buffer.Resize(image->width * image->height * image->channels);
    input->read_image(0, 0, 0, image->channels, OIIO::TypeDesc::UINT8, image->Writable());
    input->close();

    /// Update the internals as well here
    /// This method is called for Thumbnail
    m_Width = spec.width;
    m_Height = spec.height;
    m_Channels = spec.nchannels;

    /// Color specs
    std::string_view colorspace = spec.get_string_attribute("oiio:ColorSpace");
    if (!colorspace.empty())
    {
        // VOID_LOG_INFO("Colorspace: {}", colorspace);
        if (colorspace == "sRGB")
            m_ColorInfo.colorspace = Colorspace::SRGB;
        else if (colorspace == "Rec709")
            m_ColorInfo.colorspace = Colorspace::Rec709;
        else if (colorspace == "AdobeRGB")
            m_ColorInfo.colorspace = Colorspace::AdobeRGB;
        else if (colorspace == "ACEScg" || colorspace == "lin_ap1_scene")
            m_ColorInfo.colorspace = Colorspace::ACEScg;
        else if (colorspace == "lin_ap0_scene")
            m_ColorInfo.colorspace = Colorspace::ACES2065_1;
        else if (colorspace == "lin_rec709" || colorspace == "lin_rec709_scene" || colorspace == "Linear")
            m_ColorInfo.colorspace = Colorspace::LinearRec709;
        else
            m_ColorInfo.colorspace = Colorspace::Custom;
    }

    const OIIO::ParamValue* icc = spec.find_attribute("ICCProfile");
    if (icc)
        m_ColorInfo.iccprofile = true;

    const float gamma = spec.get_float_attribute("oiio:Gamma");
    // VOID_LOG_INFO("Gamma: {}", gamma);
    if (gamma > 0.f)
    {
        if (std::abs(gamma - 2.2f) < 0.01f)
            m_ColorInfo.transfer = TransferFunction::Gamma22;
        else if (std::abs(gamma - 2.4f) < 0.01f)
            m_ColorInfo.transfer = TransferFunction::Gamma24;
    }

    return true;
}

bool OIIOMediaReader::Read(const std::string& path, v_frame_t framenumber, FloatImage& image)
{
    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(path);
    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", path);
        // Fetch the error from OpenImageIO
        VOID_LOG_ERROR(OIIO::geterror());
        return false;
    }

    // Read image spec
    const OIIO::ImageSpec spec = input->spec();
    image->width = spec.width;
    image->height = spec.height;
    image->channels = spec.nchannels;
    image->format = (image->channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA;
    image->type = VOID_GL_FLOAT;

    image->buffer.Resize(image->width * image->height * image->channels);

    input->read_image(0, 0, 0, image->channels, OIIO::TypeDesc::FLOAT, image->Writable());
    input->close();

    return true;
}

std::map<std::string, std::string> OIIOMediaReader::Metadata() const
{
    std::map<std::string, std::string> m;

    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(m_Path);
    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", m_Path);
        // Log the original error from OpenImageIO
        VOID_LOG_ERROR(OIIO::geterror());
        return m;
    }

    const OIIO::ImageSpec spec = input->spec();

    // Basic
    m["filepath"] = m_Path;
    m["width"] = std::to_string(spec.width);
    m["height"] = std::to_string(spec.height);
    m["channels"] = std::to_string(spec.nchannels);
    m["colorspace"] = spec.get_string_attribute("oiio::ColorSpace");
    m["bit_depth"] = std::to_string(spec.get_int_attribute("BitsPerSample", 8));
    m["format"] = spec.format.c_str();

    // Additional
    for (const OIIO::ParamValue& attr : spec.extra_attribs)
    {
        if (attr.type() == OIIO::TypeDesc::STRING)
            m[attr.name().c_str()] = *(const char**)attr.data();
    }

    return m;   
}

VOID_NAMESPACE_CLOSE
