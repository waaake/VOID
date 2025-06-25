/* OpenImageIO */
#include "OpenImageIO/imageio.h"

/* Internal */
#include "OIIOReader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

OIIOPixReader::OIIOPixReader(const std::string& path)
    : m_Path(path)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
{
}

OIIOPixReader::OIIOPixReader()
    : OIIOPixReader("")
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

void OIIOPixReader::Read(const std::string& path, int framenumber)
{
    m_Path = path;

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

    VOID_LOG_INFO("OIIOPixReader ( Width: {0}, Height: {1}, Channels: {2} )", m_Width, m_Height, m_Channels);

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
