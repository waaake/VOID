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
{
}

TurboJpegReader::~TurboJpegReader()
{
    Clear();
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
    if (tjDecompress2(handle, jpegBuffer.data(), jpegSize, image->Writable(), image->width, pitch, image->height, TJPF_RGBA, TJFLAG_FASTDCT) != 0)
    {
        VOID_LOG_ERROR("Failed to decompress JPEG: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    tjDestroy(handle);
}

void TurboJpegReader::Clear()
{
    m_Image->Clear();
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
    std::vector<unsigned char> pixels(pitch * image->height);

    if (tjDecompress2(handle, jpegBuffer.data(), jpegSize, pixels.data(), image->width, pitch, image->height, TJPF_RGBA, TJFLAG_FASTDCT) != 0)
    {
        VOID_LOG_ERROR("Failed to decompress JPEG: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    tjDestroy(handle);

    for (std::size_t i = 0; i < (image->width * image->height); ++i)
    {
        int index = i * image->channels;
        image->buffer[index] = Linear(static_cast<float>(pixels[index]) / 255.f);
        image->buffer[index + 1] = Linear(static_cast<float>(pixels[index + 1]) / 255.f);
        image->buffer[index + 2] = Linear(static_cast<float>(pixels[index + 2]) / 255.f);
        image->buffer[index + 3] = Linear(static_cast<float>(pixels[index + 3]) / 255.f);
    }
}

void TurboJpegReader::Read()
{
    std::ifstream file(m_Path, std::ios::binary);
    if (!file)
    {
        VOID_LOG_ERROR("Cannot Open file: {0}", m_Path);
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
    if (tjDecompressHeader3(handle, jpegBuffer.data(), jpegSize, &m_Image->width, &m_Image->height, &subsample, &colorspace) != 0)
    {
        VOID_LOG_ERROR("Failed to read JPEG header: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    // Use RGBA
    m_Image->channels = tjPixelSize[TJPF_RGBA];
    int pitch = m_Image->width * m_Image->channels;

    m_Image->type = VOID_GL_FLOAT;
    m_Image->format = VOID_GL_RGBA;

    VOID_LOG_INFO("Float TurboJPEG Reader: Height {0}, Width {1}, Channels: {2}", m_Image->width, m_Image->height, m_Image->channels);

    m_Image->buffer.Resize(pitch * m_Image->height);
    std::vector<unsigned char> pixels(pitch * m_Image->height);

    if (tjDecompress2(handle, jpegBuffer.data(), jpegSize, pixels.data(), m_Image->width, pitch, m_Image->height, TJPF_RGBA, TJFLAG_FASTDCT) != 0)
    {
        VOID_LOG_ERROR("Failed to decompress JPEG: {0}", tjGetErrorStr());

        tjDestroy(handle);
        return;
    }

    tjDestroy(handle);

    for (std::size_t i = 0; i < (m_Image->width * m_Image->height); ++i)
    {
        int index = i * m_Image->channels;
        m_Image->buffer[index] = Linear(static_cast<float>(pixels[index]) / 255.f);
        m_Image->buffer[index + 1] = Linear(static_cast<float>(pixels[index + 1]) / 255.f);
        m_Image->buffer[index + 2] = Linear(static_cast<float>(pixels[index + 2]) / 255.f);
        m_Image->buffer[index + 3] = Linear(static_cast<float>(pixels[index + 3]) / 255.f);
    }
}

const std::map<std::string, std::string> TurboJpegReader::Metadata() const
{
    std::map<std::string, std::string> m;
    return m;
}

VOID_NAMESPACE_CLOSE
