// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring>

/* OpenImageIO */
#include <OpenImageIO/imageio.h>

/* Internal */
#include "OIIOWriter.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

OIIOWriter::OIIOWriter(const EncodeSpec& spec)
    : PixWriter(spec)
    , m_OutPtr(nullptr)
{
}

bool OIIOWriter::Setup(const std::string& path)
{
    m_OutPtr = OIIO::ImageOutput::create(path);
    if (!m_OutPtr)
    {
        VOID_LOG_WARN("OIIOWriter::Unable to create out file at: {0}", path);
        return false;
    }

    OIIO::ImageSpec spec(m_Spec.outwidth, m_Spec.outheight, m_Spec.channels, OIIO::TypeDesc::UINT8);
    m_OutPtr->open(path, spec);

    return true;
}

bool OIIOWriter::AddBuffer(const void* buffer, std::size_t size, const InputSpec& spec)
{
    if (m_OutPtr)
    {
        switch (spec.type)
        {
            case BufferType::Uint16:
                m_OutPtr->write_image(OIIO::TypeDesc::UINT16, buffer);
                break;
            case BufferType::Float:
                m_OutPtr->write_image(OIIO::TypeDesc::FLOAT, buffer);
                break;
            case BufferType::Uint8:
            default:
                m_OutPtr->write_image(OIIO::TypeDesc::UINT8, buffer);
        }

        VOID_LOG_INFO("Image written to file");
        return true;
    }

    return false;
}

bool OIIOWriter::Write()
{
    return (bool)m_OutPtr;
}

void OIIOWriter::Cleanup()
{
    if (m_OutPtr)
        m_OutPtr->close();
}

VOID_NAMESPACE_CLOSE
