// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _OIIO_PIX_WRITER_H
#define _OIIO_PIX_WRITER_H

/* OpenImageIO */
#include <OpenImageIO/imageio.h>

/* Internal */
#include "Definition.h"
#include "PixWriter.h"

VOID_NAMESPACE_OPEN

class VOID_API OIIOWriter : public PixWriter
{
public:
    OIIOWriter(int width, int height, int channels, const WriterType& type);

    bool Setup(const std::string& path) override;
    bool AddBuffer(const void* buffer, std::size_t size, const BufferType& type) override;
    bool Write() override;
    void Cleanup() override;

private:
    OIIO::ImageOutput::unique_ptr m_OutPtr;
};

VOID_NAMESPACE_CLOSE

#endif // _OIIO_PIX_WRITER_H
