// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* OpenImageIO */
#include "OpenImageIO/imageio.h"
#include "OpenImageIO/imagebufalgo.h"

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
            : ImageRow(m_Pixels.data(), row, m_Width, m_Channels, sizeof(unsigned char));
}

void OIIOPixReader::Read(std::size_t downscale)
{
    if (downscale > 1)
        return ScaledRead(downscale);

    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(m_Path);
    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", m_Path);

        /* Log the original error from OpenImageIO */
        VOID_LOG_ERROR(OIIO::geterror());
        return;
    }

    const OIIO::ImageSpec spec = input->spec();

    m_Width = spec.width;
    m_Height = spec.height;
    m_Channels = spec.nchannels;

    std::string_view colorspace = spec.get_string_attribute("oiio:ColorSpace");
    if (colorspace.find("Rec.709") != std::string_view::npos)
        m_InputColorSpace = ColorSpace::Rec709;

    // VOID_LOG_INFO("OIIOPixReader ( Width: {0}, Height: {1}, Channels: {2} )", m_Width, m_Height, m_Channels);

    /* Read requisites */
    int subimage = 0;
    int miplevel = 0;
    /* Channels to read from - to */
    int chbegin = 0, chend = m_Channels;

    m_Pixels.resize(m_Width * m_Height * m_Channels);
    input->read_image(subimage, miplevel, chbegin, chend, OIIO::TypeDesc::UINT8, m_Pixels.data());

    input->close();
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

void OIIOPixReader::ScaledRead(std::size_t downscale)
{
    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(m_Path);
    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", m_Path);

        /* Log the original error from OpenImageIO */
        VOID_LOG_ERROR(OIIO::geterror());
        return;
    }

    const OIIO::ImageSpec spec = input->spec();

    m_Width = spec.width / downscale;
    m_Height = spec.height / downscale;
    m_Channels = spec.nchannels;

    std::vector<unsigned char> raw(spec.width * spec.height * m_Channels);

    std::string_view colorspace = spec.get_string_attribute("oiio:ColorSpace");
    if (colorspace.find("Rec.709") != std::string_view::npos)
        m_InputColorSpace = ColorSpace::Rec709;

    /* Read requisites */
    int subimage = 0;
    int miplevel = 0;
    /* Channels to read from - to */
    int chbegin = 0, chend = m_Channels;

    input->read_image(subimage, miplevel, chbegin, chend, OIIO::TypeDesc::UINT8, raw.data());
    input->close();

    OIIO::ImageSpec src(spec.width, spec.height, m_Channels, OIIO::TypeDesc::UINT8);
    OIIO::ImageBuf srcbuf(src, raw.data());

    m_Pixels.resize(m_Width * m_Height * m_Channels);

    OIIO::ImageSpec dstspec(m_Width, m_Height, m_Channels, OIIO::TypeDesc::UINT8);
    OIIO::ImageBuf dstbuf(dstspec, m_Pixels.data());

    OIIO::ROI roi(0, m_Width, 0, m_Height, 0, 1, 0, m_Channels);

    bool resized = OIIO::ImageBufAlgo::resize(dstbuf, srcbuf, "", 0.f, roi);
    if (!resized)
    {
        VOID_LOG_ERROR("Unable to resize image: {}", OIIO::geterror());
        m_Width = spec.width;
        m_Height = spec.height;
        std::swap(raw, m_Pixels);
    }
}

VOID_NAMESPACE_CLOSE
