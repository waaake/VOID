// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <fstream>

/* TurboJPEG */
#include <turbojpeg.h>

/* Internal */
#include "TurboJpegReader.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

TurboJpegReader::TurboJpegReader(const std::string& path, v_frame_t framenumber)
    : VoidPixReader(path, framenumber)
    , m_Width(0)
    , m_Height(0)
    , m_Channels(0)
    , m_InputColorSpace(ColorSpace::sRGB)
{
}

TurboJpegReader::~TurboJpegReader()
{
    Clear();
}

void TurboJpegReader::Clear()
{
    /* Remove any data from the pixels vector and shrink it back in place */
    m_Pixels.clear();
    m_Pixels.shrink_to_fit();
}

void TurboJpegReader::Read()
{
    /* Load JPEG */
    std::ifstream file(m_Path, std::ios::binary);

    if (!file)
    {
        VOID_LOG_ERROR("Cannot Open file: {0}", m_Path);
        return;
    }
    
    /* Determine the file size */
    file.seekg(0, std::ios::end);
    size_t jpegSize = file.tellg();
    /* Reset */
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> jpegBuffer(jpegSize);
    file.read(reinterpret_cast<char*>(jpegBuffer.data()), jpegSize);
    file.close();
    
    tjhandle handle = tjInitDecompress();

    if (!handle)
    {
        VOID_LOG_ERROR("Turbo JPEG init failed.");
        return;
    }

    int subsample, colorspace;

    /* Try to read the jpeg specs */
    if (tjDecompressHeader3(handle, jpegBuffer.data(), jpegSize, &m_Width, &m_Height, &subsample, &colorspace) != 0)
    {
        VOID_LOG_ERROR("Failed to read JPEG header: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    /* Use RGBA */
    m_Channels = tjPixelSize[TJPF_RGBA];
    int pitch = m_Width * m_Channels;

    VOID_LOG_INFO("TurboJPEG Reader: Height {0}, Width {1}, Channels: {2}", m_Width, m_Height, m_Channels);

    m_Pixels.resize(pitch * m_Height);

    if (tjDecompress2(handle, jpegBuffer.data(), jpegSize, m_Pixels.data(), m_Width, pitch, m_Height, TJPF_RGBA, TJFLAG_FASTDCT) != 0)
    {
        VOID_LOG_ERROR("Failed to decompress JPEG: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    tjDestroy(handle);
}

const std::map<std::string, std::string> TurboJpegReader::Metadata() const
{
    std::map<std::string, std::string> m;
    return m;
}

VOID_NAMESPACE_CLOSE
