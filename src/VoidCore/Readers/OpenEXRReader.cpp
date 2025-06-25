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
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfChannelList.h>

/* Internal */
#include "OpenEXRReader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

OpenEXRReader::OpenEXRReader(const std::string& path)
    : m_Path(path)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
{
}

OpenEXRReader::OpenEXRReader()
    : OpenEXRReader("")
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

void OpenEXRReader::Read(const std::string& path, int framenumber)
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
}

VOID_NAMESPACE_CLOSE
