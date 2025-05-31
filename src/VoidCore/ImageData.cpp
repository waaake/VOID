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

    /* 
     * As we have the image read
     * Get the ImageSpecs from it
     */
    const OIIO::ImageSpec spec = input->spec();

    /* Update the specs */
    m_Width = spec.width;
    m_Height = spec.height;
    m_Channels = spec.nchannels;

    VOID_LOG_INFO("OIIOImage ( Width: {0}, Height: {1}, Channels: {2})", m_Width, m_Height, m_Channels);

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

VOID_NAMESPACE_CLOSE
