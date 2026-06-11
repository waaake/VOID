// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cstring>

/* OpenImageIO */
#include <OpenImageIO/imageio.h>
#include <OpenImageIO/imagebufalgo.h>

/* Internal */
#include "OIIOWriter.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

OIIOWriter::OIIOWriter(const EncodeSpec& spec)
    : PixWriter(spec)
{
}

bool OIIOWriter::Setup(const std::string& path)
{
    m_Path = path;
    return true;
}

bool OIIOWriter::AddBuffer(const void* buffer, std::size_t size, const InputSpec& spec)
{
    bool status = false;
    OIIO::ROI roi(0, m_Spec.width, 0, m_Spec.height, 0, 1, 0, m_Spec.channels);

    switch (spec.type)
    {
        case BufferType::Uint16:
        {
            OIIO::ImageSpec inspec(spec.width, spec.height, spec.channels, OIIO::TypeDesc::UINT16);
            OIIO::ImageBuf inbuf(inspec, const_cast<void*>(buffer));

            std::vector<unsigned short> out(m_Spec.outwidth * m_Spec.outheight * m_Spec.channels);
            OIIO::ImageSpec outspec(m_Spec.outwidth, m_Spec.outheight, m_Spec.channels, OIIO::TypeDesc::UINT16);
            OIIO::ImageBuf outbuf(outspec, out.data());

            status = OIIO::ImageBufAlgo::resize(outbuf, inbuf, "", 0.f, roi);
            if (!status)
            {
                VOID_LOG_ERROR("Unable to resize image: {0}", outbuf.geterror());
                return false;
            }
            
            return outbuf.write(m_Path);
        }
        case BufferType::Float:
        {

            OIIO::ImageSpec inspec(spec.width, spec.height, spec.channels, OIIO::TypeDesc::FLOAT);
            OIIO::ImageBuf inbuf(inspec, const_cast<void*>(buffer));

            std::vector<float> out(m_Spec.outwidth * m_Spec.outheight * m_Spec.channels);
            OIIO::ImageSpec outspec(m_Spec.outwidth, m_Spec.outheight, m_Spec.channels, OIIO::TypeDesc::FLOAT);
            OIIO::ImageBuf outbuf(outspec, out.data());

            status = OIIO::ImageBufAlgo::resize(outbuf, inbuf, "", 0.f, roi);
            if (!status)
            {
                VOID_LOG_ERROR("Unable to resize image: {0}", outbuf.geterror());
                return false;
            }
            
            return outbuf.write(m_Path);
        }
        case BufferType::Uint8:
        default:
        {
            OIIO::ImageSpec inspec(spec.width, spec.height, spec.channels, OIIO::TypeDesc::UINT8);
            OIIO::ImageBuf inbuf(inspec, const_cast<void*>(buffer));

            std::vector<unsigned char> out(m_Spec.outwidth * m_Spec.outheight * m_Spec.channels);
            OIIO::ImageSpec outspec(m_Spec.outwidth, m_Spec.outheight, m_Spec.channels, OIIO::TypeDesc::UINT8);
            OIIO::ImageBuf outbuf(outspec, out.data());

            status = OIIO::ImageBufAlgo::resize(outbuf, inbuf, "", 0.f, roi);
            if (!status)
            {
                VOID_LOG_ERROR("Unable to resize image: {0}", outbuf.geterror());
                return false;
            }
            
            return outbuf.write(m_Path);
        }
    }

    return status;
}

bool OIIOWriter::Write()
{
    return true;
}

void OIIOWriter::Cleanup()
{
}

VOID_NAMESPACE_CLOSE
