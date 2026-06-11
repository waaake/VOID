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
    OIIOWriter(const EncodeSpec& spec);

    bool Setup(const std::string& path) override;
    bool AddBuffer(const void* buffer, std::size_t size, const InputSpec& spec) override;
    bool Write() override;
    void Cleanup() override;

private:
    std::string m_Path;
};

VOID_NAMESPACE_CLOSE

#endif // _OIIO_PIX_WRITER_H
