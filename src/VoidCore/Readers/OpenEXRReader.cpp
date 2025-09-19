// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifdef _WIN32
/* https://github.com/AcademySoftwareFoundation/Imath/issues/212 
 * This needs to be declared else we get unresolved symbol from imath library
 * unresolved external symbol imath_half_to_float_table
 * The declaration of imath_half_to_float_table picks up the extern from the extern "C"
 */
#define IMATH_HALF_NO_LOOKUP_TABLE
#endif

/* STD */
#include <algorithm>

/* OpenEXR */
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfCompression.h>
#include <OpenEXR/ImfFloatAttribute.h>
#include <OpenEXR/ImfHeader.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfIntAttribute.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfStringAttribute.h>

/* Internal */
#include "OpenEXRReader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

OpenEXRReader::OpenEXRReader(const std::string& path, v_frame_t framenumber)
    : VoidPixReader(path, framenumber)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
{
}

OpenEXRReader::~OpenEXRReader()
{
    Clear();
}

void OpenEXRReader::Clear()
{
    /* Remove any data from the pixels vector and shrink it back in place */
    m_Pixels.clear();
    m_Pixels.shrink_to_fit();
}

const unsigned char* OpenEXRReader::ThumbnailPixels()
{
    /* This is always assuming that the frame has been read */
    if (m_TPixels.empty())
    {
        /* Reserve the size on Thumbnail Data */
        m_TPixels.reserve(m_Pixels.size());

        unsigned char* dest = m_TPixels.data();

        for (size_t i = 0; i < (m_Width * m_Height); ++i)
        {
            int index = i * 4;
            dest[index + 0] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 0], 0.f, 1.f) * 255.f);
            dest[index + 1] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 1], 0.f, 1.f) * 255.f);
            dest[index + 2] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 2], 0.f, 1.f) * 255.f);
            dest[index + 3] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 3], 0.f, 1.f) * 255.f);
        }
    }

    /* Return the Thumbnail data */
    return m_TPixels.data();
}

void OpenEXRReader::Read()
{
    /* Create an EXR Reader */
    Imf::RgbaInputFile f(m_Path.c_str());

    /* Get Image Specifications */
    Imath::Box2i dw = f.dataWindow();
    m_Width = (dw.max.x - dw.min.x) + 1;
    m_Height = (dw.max.y - dw.min.y) + 1;

    /* To Get the channels -> Read through the header */
    const Imf::Header& header = f.header();
    const Imf::ChannelList& channels = header.channels();

    // /*
    //  * TODO: Check if we have a better way of reading channels rather than iterating here ?
    //  * Update the channel count by reading the channels
    //  */
    // for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++ it)
    //     m_Channels++;

    /* Force 4 Channels */
    m_Channels = 4;

    VOID_LOG_INFO("EXRImage ( Width: {0}, Height: {1}, Channels: {2} )", m_Width, m_Height, m_Channels);

    /* Allocate the Pixel buffer for the EXR Image */
    Imf::Array2D<Imf::Rgba> pixels(m_Height, m_Width);

    /* Read the Pixel data onto the buffer */
    f.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * m_Width, 1, m_Width);
    /* Read the scanlines */
    f.readPixels(dw.min.y, dw.max.y);

    /* 
     * Once we have the pixel data from the EXR 
     * Cast this data into unsigned char buffer
     * TODO: Check if we can use this data onto Renderer directly without casting this ?
     */
    /* Convert floating point exr pixels to 8-bit RGB/RGBA format */
    m_Pixels.resize(m_Width * m_Height * m_Channels);

    for (int y = 0; y < m_Height; y++)
    {
        for (int x = 0; x < m_Width; x++)
        {
            /* The pixel from the buffer */
            const Imf::Rgba& pixel = pixels[y][x];
            /* Find the index at which we will be writing to on the pixel buffer */
            int index = (y * m_Width + x) * m_Channels;

            m_Pixels[index] = pixel.r;
            m_Pixels[index + 1] = pixel.g;
            m_Pixels[index + 2] = pixel.b;
            m_Pixels[index + 3] = pixel.a;
        }
    }
}

const std::map<std::string, std::string> OpenEXRReader::Metadata() const
{
    std::map<std::string, std::string> m;

    Imf::InputFile f(m_Path.c_str());
    const Imf::Header& header = f.header();

    /* Basic Metadata */
    m["filepath"] = m_Path;

    int channelCount = 0;
    const Imf::ChannelList& channels = header.channels();

    Imf::PixelType pixelType = Imf::PixelType::HALF;
    for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++it)
    {
        channelCount++;
        pixelType = it.channel().type;
    }

    /**
     * UINT  = 0, // unsigned int (32 bit)
     * HALF  = 1, // half (16 bit floating point)
     * FLOAT = 2, // float (32 bit floating point)
     */
    if (pixelType == Imf::PixelType::HALF)
        m["pixelType"] = "16 bit floating point";
    else if (pixelType == Imf::PixelType::FLOAT)
        m["pixelType"] = "32 bit floating point";
    else
        m["pixelType"] = "32 bit";

    Imath::Box2i dw = header.dataWindow();
    int width = dw.max.x - dw.min.x + 1;
    int height = dw.max.y - dw.min.y + 1;

    m["width"] = std::to_string(width);
    m["height"] = std::to_string(height);
    m["channels"] = std::to_string(channelCount);

    // const Imf::Compression& compression = header.compression();
    // std::string compressionType;
    // Imf::getCompressionNameFromId(compression, compressionType);
    // m["compression"] = compressionType;

    /* Specific metadata */
    for (Imf::Header::ConstIterator it = header.begin(); it != header.end(); ++it)
    {
        const Imf::Attribute& attr = it.attribute();

        if (std::strcmp(attr.typeName(), "string") == 0)
        {
            const Imf::StringAttribute& sattr = static_cast<const Imf::StringAttribute&>(attr);
            m[it.name()] = sattr.value();
        }
        else if (std::strcmp(attr.typeName(), "float") == 0)
        {
            const Imf::FloatAttribute& fattr = static_cast<const Imf::FloatAttribute&>(attr);
            m[it.name()] = std::to_string(fattr.value());
        }
        else if (std::strcmp(attr.typeName(), "int") == 0)
        {
            const Imf::IntAttribute& iattr = static_cast<const Imf::IntAttribute&>(attr);
            m[it.name()] = std::to_string(iattr.value());
        }
    }

    return m;
}

VOID_NAMESPACE_CLOSE
