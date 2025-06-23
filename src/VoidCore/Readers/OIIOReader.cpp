/* OpenImageIO */
#include <OpenImageIO/imageio.h>

/* Internal */
#include "OIIOReader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

/* OIIOReader {{{ */

OIIOPixBlock::OIIOPixBlock(const std::vector<unsigned char> pixels, const int width, const int height, const int channels)
    : m_Pixels(pixels)
    , m_Width(width)
    , m_Height(height)
    , m_Channels(channels)
    , m_Format(channels == 3 ? VOID_GL_RGB : VOID_GL_RGBA)
{
}

OIIOPixBlock::~OIIOPixBlock()
{
    /* Clear any data from the pixels */
    Clear();
}

/* }}}*/

/* OIIOReader {{{ */

SharedPixBlock OIIOReader::Read(const std::string& path)
{
    std::unique_ptr<OIIO::ImageInput> input = OIIO::ImageInput::open(path);

    if (!input)
    {
        VOID_LOG_INFO("Unable to load image. Path: {0}", path);

        /* Log the original error from OpenImageIO */
        VOID_LOG_ERROR(OIIO::geterror());
        return nullptr;
    }

    /* 
     * As we have the image read
     * Get the ImageSpecs from it
     */
    std::vector<unsigned char> pixels;
    const OIIO::ImageSpec spec = input->spec();

    /* Update the specs */
    int width = spec.width;
    int height = spec.height;
    int channels = spec.nchannels;

    VOID_LOG_INFO("OIIOImage ( Width: {0}, Height: {1}, Channels: {2} )", width, height, channels);

    /* And since we're here and able to read the image -> update the internal state that the image has been read */
    // m_Empty = false;

    /* Read requisites */
    int subimage = 0;
    int miplevel = 0;
    /* Channels to read from - to */
    int chbegin = 0, chend = channels;

    /* Resize the Pixels */
    pixels.resize(width * height * channels);

    /* Read the image pixels */
    input->read_image(subimage, miplevel, chbegin, chend, OIIO::TypeDesc::UINT8, pixels.data());

    /* Close the image after reading */
    input->close();

    /* Construct the OIIO PixBlock */
    std::shared_ptr<OIIOPixBlock> block = std::make_shared<OIIOPixBlock>(pixels, width, height, channels);
    // OIIOPixBlock* block = new OIIOPixBlock(pixels, width, height, channels);

    return block;
}

/* }}}*/

VOID_NAMESPACE_CLOSE
