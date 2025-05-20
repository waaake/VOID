/* Internal */
#include "ImageData.h"

VOID_NAMESPACE_OPEN

VoidImageData::VoidImageData()
    : m_Width(0)
    , m_Height(0)
    , m_Channels(0)
    , m_Data(nullptr)
{
}

VoidImageData::VoidImageData(const std::string& path)
{
    Read(path);
}

VoidImageData::~VoidImageData()
{
    if (m_Data)
    {
        // delete m_Data;
        // m_Data = nullptr;
        // stbi_image_free(m_Data);
    }
}

void VoidImageData::Read(const std::string& path)
{
    /* Load the data from the provided image */
    m_Data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, STBI_rgb_alpha);
}

void VoidImageData::Free()
{
    if (m_Data)
    {
        /* Just calls free on the internal data */
        stbi_image_free(m_Data);
        m_Data = nullptr;
    }
}

VOID_NAMESPACE_CLOSE
