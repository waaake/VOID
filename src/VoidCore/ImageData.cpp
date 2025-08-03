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

/* OpenEXR */
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfChannelList.h>

/* Internal */
#include "ImageData.h"
#include "Logging.h"

VOID_NAMESPACE_OPEN

VoidImageData::VoidImageData()
    : m_Width(0)
    , m_Height(0)
    , m_Channels(0)
    , m_Empty(true)
{
}

VoidImageData::VoidImageData(const std::string& path)
{
    Read(path);
}

VoidImageData::~VoidImageData()
{
    /* Flush out any data from the vector */
    Free();
}

void VoidImageData::Read(const std::string& path)
{

    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(path);

    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", path);

        /* Log the original error from OpenImageIO */
        VOID_LOG_ERROR(OIIO::geterror());
        return;
    }

    /* Check if it is an EXR Image */
    std::string format = input->format_name();
    if (format == "openexr")
    {
        /* Since this is an exr frame -> close the ImageInput */
        input->close();
        /* Read using OpenEXR */
        ReadFromEXR(path);
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

    VOID_LOG_INFO("OIIOImage ( Width: {0}, Height: {1}, Channels: {2} )", m_Width, m_Height, m_Channels);

    /* And since we're here and able to read the image -> update the internal state that the image has been read */
    m_Empty = false;

    /* Read requisites */
    int subimage = 0;
    int miplevel = 0;
    /* Channels to read from - to */
    int chbegin = 0, chend = m_Channels;

    /* Resize the Pixels */
    m_Pixels.resize(m_Width * m_Height * m_Channels);

    /* Read the image pixels */
    input->read_image(subimage, miplevel, chbegin, chend, OIIO::TypeDesc::UINT8, m_Pixels.data());

    /* Close the image after reading */
    input->close();
}

void VoidImageData::ReadFromEXR(const std::string& path)
{
    /* Create an EXR Reader */
    Imf::RgbaInputFile f(path.c_str());

    /* Get Image Specifications */
    Imath::Box2i dw = f.dataWindow();
    m_Width = (dw.max.x - dw.min.x) + 1;
    m_Height = (dw.max.y - dw.min.y) + 1;

    /* To Get the channels -> Read through the header */
    const Imf::Header& header = f.header();
    const Imf::ChannelList& channels = header.channels();

    /*
     * TODO: Check if we have a better way of reading channels rather than iterating here ?
     * Update the channel count by reading the channels
     */
    for (Imf::ChannelList::ConstIterator it = channels.begin(); it != channels.end(); ++ it)
        m_Channels++;

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

            /* Convert half values into unsigned char (0-255) */
            m_Pixels[index] = static_cast<unsigned char>(std::clamp(pixel.r * 255.f, 0.f, 255.f)); /* Red */
            m_Pixels[index + 1] = static_cast<unsigned char>(std::clamp(pixel.g * 255.f, 0.f, 255.f)); /* Green */
            m_Pixels[index + 2] = static_cast<unsigned char>(std::clamp(pixel.b * 255.f, 0.f, 255.f)); /* Blue */

            /* In case we have Alpha channel, update the value accordingly */
            if (m_Channels == 4)
            {
                m_Pixels[index + 3] = static_cast<unsigned char>(std::clamp(pixel.a * 255.f, 0.f, 255.f)); /* Alpha */
            }
        }
    }

    /* Update the state of the data which has been read */
    m_Empty = false;
}

VOID_NAMESPACE_CLOSE
