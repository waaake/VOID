// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>
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

const unsigned char* TurboJpegReader::ThumbnailPixels()
{
    if (m_TPixels.empty())
    {
        m_TPixels.resize(m_Pixels.size());
        unsigned char* pixels = m_TPixels.data();

        for (std::size_t i = 0; i < (m_Width * m_Height); ++i)
        {
            int index = i * m_Channels;

            pixels[index] = static_cast<unsigned char>(std::clamp(m_Pixels[index], 0.f, 1.f) * 255.f);
            pixels[index + 1] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 1], 0.f, 1.f) * 255.f);
            pixels[index + 2] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 2], 0.f, 1.f) * 255.f);
            pixels[index + 3] = static_cast<unsigned char>(std::clamp(m_Pixels[index + 3], 0.f, 1.f) * 255.f);
        }
    }

    return m_TPixels.data();
}

void TurboJpegReader::ReadThumbnail(const std::string& path, v_frame_t frame, UInt8Image& image)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        VOID_LOG_ERROR("Cannot Open file: {0}", path);
        return;
    }

    // Determine the file size
    file.seekg(0, std::ios::end);
    size_t jpegSize = file.tellg();
    // Reset
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
    if (tjDecompressHeader3(handle, jpegBuffer.data(), jpegSize, &image->width, &image->height, &subsample, &colorspace) != 0)
    {
        VOID_LOG_ERROR("Failed to read JPEG header: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    // Use RGBA
    image->channels = tjPixelSize[TJPF_RGBA];
    int pitch = image->width * image->channels;

    VOID_LOG_INFO("Float TurboJPEG Reader: Height {0}, Width {1}, Channels: {2}", image->width, image->height, image->channels);

    image->buffer.Resize(pitch * image->height);
    if (tjDecompress2(handle, jpegBuffer.data(), jpegSize, image->buffer.Data(), image->width, pitch, image->height, TJPF_RGBA, TJFLAG_FASTDCT) != 0)
    {
        VOID_LOG_ERROR("Failed to decompress JPEG: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    tjDestroy(handle);
}

SharedPixels TurboJpegReader::Copy() const
{
    auto copy = std::make_shared<TurboJpegReader>(m_Path, m_Framenumber);
    copy->m_InputColorSpace = m_InputColorSpace;
    copy->m_Channels = m_Channels;
    copy->m_Width = m_Width;
    copy->m_Height = m_Height;
    copy->m_Pixels = m_Pixels;

    return copy;
}

void TurboJpegReader::Clear()
{
    /* Remove any data from the pixels vector and shrink it back in place */
    m_Pixels.clear();
    m_Pixels.shrink_to_fit();
}

ImageRow TurboJpegReader::Row(std::size_t row)
{
    return (row >= m_Height)
            ? ImageRow()
            : ImageRow(m_Pixels.data(), row, m_Width, m_Channels, sizeof(float));
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
    std::vector<unsigned char> out(pitch * m_Height);

    if (tjDecompress2(handle, jpegBuffer.data(), jpegSize, out.data(), m_Width, pitch, m_Height, TJPF_RGBA, TJFLAG_FASTDCT) != 0)
    {
        VOID_LOG_ERROR("Failed to decompress JPEG: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    tjDestroy(handle);

    for (std::size_t i = 0; i < (m_Width * m_Height); ++i)
    {
        int index = i * m_Channels;
        m_Pixels[index] = Linear(static_cast<float>(out[index]) / 255.f);
        m_Pixels[index + 1] = Linear(static_cast<float>(out[index + 1]) / 255.f);
        m_Pixels[index + 2] = Linear(static_cast<float>(out[index + 2]) / 255.f);
        m_Pixels[index + 3] = Linear(static_cast<float>(out[index + 3]) / 255.f);
    }
}

void TurboJpegReader::Read(const std::string& path, v_frame_t frame, FloatImage& image)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
    {
        VOID_LOG_ERROR("Cannot Open file: {0}", path);
        return;
    }

    // Determine the file size
    file.seekg(0, std::ios::end);
    size_t jpegSize = file.tellg();
    // Reset
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
    if (tjDecompressHeader3(handle, jpegBuffer.data(), jpegSize, &image->width, &image->height, &subsample, &colorspace) != 0)
    {
        VOID_LOG_ERROR("Failed to read JPEG header: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    // Use RGBA
    image->channels = tjPixelSize[TJPF_RGBA];
    int pitch = image->width * image->channels;

    image->type = VOID_GL_FLOAT;
    image->format = VOID_GL_RGBA;

    VOID_LOG_INFO("Float TurboJPEG Reader: Height {0}, Width {1}, Channels: {2}", image->width, image->height, image->channels);

    image->buffer.Resize(pitch * image->height);
    std::vector<unsigned char> out(pitch * image->height);

    if (tjDecompress2(handle, jpegBuffer.data(), jpegSize, out.data(), image->width, pitch, image->height, TJPF_RGBA, TJFLAG_FASTDCT) != 0)
    {
        VOID_LOG_ERROR("Failed to decompress JPEG: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    tjDestroy(handle);

    for (std::size_t i = 0; i < (image->width * image->height); ++i)
    {
        int index = i * image->channels;
        image->buffer[index] = Linear(static_cast<float>(out[index]) / 255.f);
        image->buffer[index + 1] = Linear(static_cast<float>(out[index + 1]) / 255.f);
        image->buffer[index + 2] = Linear(static_cast<float>(out[index + 2]) / 255.f);
        image->buffer[index + 3] = Linear(static_cast<float>(out[index + 3]) / 255.f);
    }
}

const std::map<std::string, std::string> TurboJpegReader::Metadata() const
{
    std::map<std::string, std::string> m;
    return m;
}

VOID_NAMESPACE_CLOSE
