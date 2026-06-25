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

OIIOPixReader::OIIOPixReader(const std::string& path, v_frame_t framenumber)
    : VoidPixReader(path, framenumber)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
    , m_InputColorSpace(ColorSpace::sRGB)
{
}

OIIOPixReader::~OIIOPixReader()
{
    Clear();
}

const unsigned char* OIIOPixReader::ThumbnailPixels()
{
    if (m_TPixels.empty())
    {
        m_TPixels.resize(m_Pixels.size());
        unsigned char* pixels = m_TPixels.data();

        for (std::size_t i = 0; i < (m_Width * m_Height); ++i)
        {
            int index = i * m_Channels;

            pixels[index] = static_cast<unsigned char>(std::clamp(m_Pixels[index], 0.f, 1.f) * 255.f);
            pixels[index + 1] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 1], 0.f, 1.f) * 255.f);
            pixels[index + 2] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 2], 0.f, 1.f) * 255.f);

            if (m_Channels == 4)
                pixels[index + 3] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 3], 0.f, 1.f) * 255.f);
        }
    }

    return m_TPixels.data();
}

void OIIOPixReader::ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image)
{
    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(path);
    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", path);
        // Fetch the error from OpenImageIO
        VOID_LOG_ERROR(OIIO::geterror());
        return;
    }

    // Read image spec
    const OIIO::ImageSpec spec = input->spec();
    image->width = spec.width;
    image->height = spec.height;
    image->channels = spec.nchannels;
    image->format = (image->channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA;
    // image->type = VOID_GL_FLOAT;

    // /* Get the colorspace from the image spec {{{ */
    // std::string_view colorspace = spec.get_string_attribute("oiio:ColorSpace");

    // /* Our default Input ColorSpace points at sRGB, only cases where we want to update that */
    // if (colorspace.find("Rec.709") != std::string_view::npos)
    //     m_InputColorSpace = ColorSpace::Rec709;
    // /* }}} */

    // VOID_LOG_INFO("OIIOPixReader ( Width: {0}, Height: {1}, Channels: {2} )", m_Width, m_Height, m_Channels);

    /* Read requisites */
    int subimage = 0;
    int miplevel = 0;
    int chbegin = 0, chend = image->channels;

    // std::vector<unsigned char> original(image->width * image->height * image->channels);
    image->buffer.Resize(image->width * image->height * image->channels);
    input->read_image(subimage, miplevel, chbegin, chend, OIIO::TypeDesc::UINT8, image->Writable());
    input->close();
}

SharedPixels OIIOPixReader::Copy() const
{
    auto copy = std::make_shared<OIIOPixReader>(m_Path, m_Framenumber);
    copy->m_InputColorSpace = m_InputColorSpace;
    copy->m_Width = m_Width;
    copy->m_Height = m_Height;
    copy->m_Channels = m_Channels;
    copy->m_Pixels = m_Pixels;

    return copy;
}

void OIIOPixReader::Clear()
{
    /* Remove any data from the pixels vector and shrink it back in place */
    m_Pixels.clear();
    m_Pixels.shrink_to_fit();
}

ImageRow OIIOPixReader::Row(std::size_t row)
{
    return (row >= m_Height)
            ? ImageRow()
            : ImageRow(m_Pixels.data(), row, m_Width, m_Channels, sizeof(float));
}

void OIIOPixReader::Read()
{
    /* As the underlying path is updated -> Invoke the actual Read */
    /* Open the file path */
    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(m_Path);

    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", m_Path);

        /* Log the original error from OpenImageIO */
        VOID_LOG_ERROR(OIIO::geterror());
        return;
    }

    /*
     * As we have the image read
     * Get the ImageSpecs from it
     */
    const OIIO::ImageSpec spec = input->spec();

    /* Update the specs */
    m_Width = spec.width;
    m_Height = spec.height;
    m_Channels = spec.nchannels;

    /* Get the colorspace from the image spec {{{ */
    std::string_view colorspace = spec.get_string_attribute("oiio:ColorSpace");

    /* Our default Input ColorSpace points at sRGB, only cases where we want to update that */
    if (colorspace.find("Rec.709") != std::string_view::npos)
        m_InputColorSpace = ColorSpace::Rec709;
    /* }}} */

    // VOID_LOG_INFO("OIIOPixReader ( Width: {0}, Height: {1}, Channels: {2} )", m_Width, m_Height, m_Channels);

    /* Read requisites */
    int subimage = 0;
    int miplevel = 0;
    int chbegin = 0, chend = m_Channels;

    std::vector<unsigned char> original(m_Width * m_Height * m_Channels);
    m_Pixels.resize(m_Width * m_Height * m_Channels);

    input->read_image(subimage, miplevel, chbegin, chend, OIIO::TypeDesc::UINT8, original.data());
    input->close();

    OIIO::ImageBuf src(spec, original.data());
    OIIO::ImageBuf linear;
    OIIO::ImageBufAlgo::colorconvert(linear, src, "sRGB", "Linear");
    linear.get_pixels(OIIO::ROI::All(), OIIO::TypeDesc::FLOAT, m_Pixels.data());
}

void OIIOPixReader::Read(const std::string& path, v_frame_t frame, FloatImage& image)
{
    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(path);
    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", path);
        // Fetch the error from OpenImageIO
        VOID_LOG_ERROR(OIIO::geterror());
        return;
    }

    // Read image spec
    const OIIO::ImageSpec spec = input->spec();
    image->width = spec.width;
    image->height = spec.height;
    image->channels = spec.nchannels;
    image->format = (image->channels == 3) ? VOID_GL_RGB : VOID_GL_RGBA;
    image->type = VOID_GL_FLOAT;

    // /* Get the colorspace from the image spec {{{ */
    // std::string_view colorspace = spec.get_string_attribute("oiio:ColorSpace");

    // /* Our default Input ColorSpace points at sRGB, only cases where we want to update that */
    // if (colorspace.find("Rec.709") != std::string_view::npos)
    //     m_InputColorSpace = ColorSpace::Rec709;
    // /* }}} */

    // VOID_LOG_INFO("OIIOPixReader ( Width: {0}, Height: {1}, Channels: {2} )", m_Width, m_Height, m_Channels);

    /* Read requisites */
    int subimage = 0;
    int miplevel = 0;
    int chbegin = 0, chend = image->channels;

    // std::vector<unsigned char> original(image->width * image->height * image->channels);
    m_TPixels.resize(image->width * image->height * image->channels);
    image->buffer.Resize(image->width * image->height * image->channels);

    input->read_image(subimage, miplevel, chbegin, chend, OIIO::TypeDesc::UINT8, m_TPixels.data());
    input->close();

    OIIO::ImageBuf src(spec, m_TPixels.data());
    OIIO::ImageBuf linear;
    OIIO::ImageBufAlgo::colorconvert(linear, src, "sRGB", "Linear");
    linear.get_pixels(OIIO::ROI::All(), OIIO::TypeDesc::FLOAT, image->Writable());
}

const std::map<std::string, std::string> OIIOPixReader::Metadata() const
{
    std::map<std::string, std::string> m;

    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(m_Path);
    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", m_Path);
        /* Log the original error from OpenImageIO */
        VOID_LOG_ERROR(OIIO::geterror());
        return m;
    }

    const OIIO::ImageSpec spec = input->spec();

    /* Basic Metadata */
    m["filepath"] = m_Path;
    m["width"] = std::to_string(spec.width);
    m["height"] = std::to_string(spec.height);
    m["channels"] = std::to_string(spec.nchannels);
    m["colorspace"] = spec.get_string_attribute("oiio::ColorSpace");
    m["bit_depth"] = std::to_string(spec.get_int_attribute("BitsPerSample", 8));
    m["format"] = spec.format.c_str();

    /* Additional */
    for (const OIIO::ParamValue& attr : spec.extra_attribs)
    {
        if (attr.type() == OIIO::TypeDesc::STRING)
            m[attr.name().c_str()] = *(const char**)attr.data();
    }

    return m;
}

VOID_NAMESPACE_CLOSE
