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
{
}

OpenEXRReader::~OpenEXRReader()
{
    Clear();
}

void OpenEXRReader::Clear()
{
    m_Image->Clear();
}

void OpenEXRReader::ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image)
{
    Imf::RgbaInputFile f(path.c_str());
    // Image Specs
    Imath::Box2i dw = f.dataWindow();
    image->width = (dw.max.x - dw.min.x) + 1;
    image->height = (dw.max.y - dw.min.y) + 1;

    /* To Get the channels -> Read through the header */
    // const Imf::Header& header = f.header();
    // const Imf::ChannelList& channels = header.channels();

    // /*
    //  * TODO: Check if we have a better way of reading channels rather than iterating here ?
    //  * Update the channel count by reading the channels
    //  */
    // for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++ it)
    //     m_Channels++;

    // Force 4 Channels
    image->channels = 4;

    // VOID_LOG_INFO("EXRImage ( Width: {0}, Height: {1}, Channels: {2} )", image->width, image->height, image->channels);

    Imf::Array2D<Imf::Rgba> pixels(image->height, image->width);

    // Read the Pixel data onto the buffer
    f.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * image->width, 1, image->width);
    f.readPixels(dw.min.y, dw.max.y);

    image->buffer.Resize(image->width * image->height * image->channels);

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            const Imf::Rgba& pixel = pixels[y][x];
            int index = (y * image->width + x) * image->channels;

            image->buffer[index] = static_cast<unsigned char>(std::clamp((float)pixel.r, 0.f, 1.f) * 255.f);
            image->buffer[index + 1] = static_cast<unsigned char>(std::clamp((float)pixel.g, 0.f, 1.f) * 255.f);
            image->buffer[index + 2] = static_cast<unsigned char>(std::clamp((float)pixel.b, 0.f, 1.f) * 255.f);
            image->buffer[index + 3] = static_cast<unsigned char>(std::clamp((float)pixel.a, 0.f, 1.f) * 255.f);
        }
    }
}

void OpenEXRReader::Read(const std::string& path, v_frame_t frame, FloatImage& image)
{
    Imf::RgbaInputFile f(path.c_str());

    // Image Specs
    Imath::Box2i dw = f.dataWindow();
    image->width = (dw.max.x - dw.min.x) + 1;
    image->height = (dw.max.y - dw.min.y) + 1;

    /* To Get the channels -> Read through the header */
    // const Imf::Header& header = f.header();
    // const Imf::ChannelList& channels = header.channels();

    // /*
    //  * TODO: Check if we have a better way of reading channels rather than iterating here ?
    //  * Update the channel count by reading the channels
    //  */
    // for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++ it)
    //     m_Channels++;

    // Force 4 Channels
    image->channels = 4;
    image->format = VOID_GL_RGBA;
    image->type = VOID_GL_FLOAT;

    // VOID_LOG_INFO("EXRImage ( Width: {0}, Height: {1}, Channels: {2} )", image->width, image->height, image->channels);

    Imf::Array2D<Imf::Rgba> pixels(image->height, image->width);

    // Read the Pixel data onto the buffer
    f.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * image->width, 1, image->width);
    f.readPixels(dw.min.y, dw.max.y);

    image->buffer.Resize(image->width * image->height * image->channels);

    for (int y = 0; y < image->height; y++)
    {
        for (int x = 0; x < image->width; x++)
        {
            const Imf::Rgba& pixel = pixels[y][x];
            int index = (y * image->width + x) * image->channels;

            image->buffer[index] = pixel.r;
            image->buffer[index + 1] = pixel.g;
            image->buffer[index + 2] = pixel.b;
            image->buffer[index + 3] = pixel.a;
        }
    }
}

void OpenEXRReader::Read()
{
    Imf::RgbaInputFile f(m_Path.c_str());

    // Image Specs
    Imath::Box2i dw = f.dataWindow();
    m_Image->width = (dw.max.x - dw.min.x) + 1;
    m_Image->height = (dw.max.y - dw.min.y) + 1;

    /* To Get the channels -> Read through the header */
    // const Imf::Header& header = f.header();
    // const Imf::ChannelList& channels = header.channels();

    // /*
    //  * TODO: Check if we have a better way of reading channels rather than iterating here ?
    //  * Update the channel count by reading the channels
    //  */
    // for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++ it)
    //     m_Channels++;

    // Force 4 Channels
    m_Image->channels = 4;
    m_Image->format = VOID_GL_RGBA;
    m_Image->type = VOID_GL_FLOAT;

    // VOID_LOG_INFO("EXRImage ( Width: {0}, Height: {1}, Channels: {2} )", m_Image->width, m_Image->height, m_Image->channels);

    Imf::Array2D<Imf::Rgba> pixels(m_Image->height, m_Image->width);

    // Read the Pixel data onto the buffer
    f.setFrameBuffer(&pixels[0][0] - dw.min.x - dw.min.y * m_Image->width, 1, m_Image->width);
    f.readPixels(dw.min.y, dw.max.y);

    m_Image->buffer.Resize(m_Image->width * m_Image->height * m_Image->channels);

    for (int y = 0; y < m_Image->height; y++)
    {
        for (int x = 0; x < m_Image->width; x++)
        {
            const Imf::Rgba& pixel = pixels[y][x];
            int index = (y * m_Image->width + x) * m_Image->channels;

            m_Image->buffer[index] = pixel.r;
            m_Image->buffer[index + 1] = pixel.g;
            m_Image->buffer[index + 2] = pixel.b;
            m_Image->buffer[index + 3] = pixel.a;
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
