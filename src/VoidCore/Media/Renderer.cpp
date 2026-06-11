// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <filesystem>

/* Internal */
#include "Renderer.h"
#include "FormatForge.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

namespace Renderer {

ImageRenderer::ImageRenderer(const MEntry& entry, const EncodeSpec& spec)
    : m_Writer(std::move(Forge::Instance().GetImageWriter(entry.Extension(), spec)))
    , m_MediaEntry(entry)
{
}

ImageRenderer::~ImageRenderer()
{
    // if (m_Writer)
    //     m_Writer->Cleanup();
}

bool ImageRenderer::AddBuffer(const void* buffer, std::size_t size, const InputSpec& spec)
{
    if (m_Writer)
    {
        if (m_Writer->Setup(m_MediaEntry.Fullpath()))
            return m_Writer->AddBuffer(buffer, size, spec);
    }

    return false;
}

bool ImageRenderer::AddBuffer(v_frame_t frame, const void* buffer, std::size_t size, const InputSpec& spec)
{
    if (m_Writer)
    {
        if (m_Writer->Setup(m_MediaEntry.ResolvedPath(frame)))
            return m_Writer->AddBuffer(buffer, size, spec);
    }

    return false;
}

bool ImageRenderer::Render()
{
    if (m_Writer)
    {
        if (m_Writer->Write())
        {
            m_Writer->Cleanup();
            return true;
        }
    }

    return false;
}

bool ImageRenderer::Render(const void* buffer, std::size_t size, const InputSpec& spec)
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("ImageRenderer::Render");
    if (m_Writer)
    {
        if (m_Writer->Setup(m_MediaEntry.Fullpath()))
        {
            bool ret = m_Writer->AddBuffer(buffer, size, spec);
            ret = m_Writer->Write();
            m_Writer->Cleanup();

            return ret;
        }
    }

    return false;
}

bool ImageRenderer::Render(v_frame_t frame, const void* buffer, std::size_t size, const InputSpec& spec)
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("ImageRenderer::Render");
    if (m_Writer)
    {
        if (m_Writer->Setup(m_MediaEntry.ResolvedPath(frame)))
        {
            bool ret = m_Writer->AddBuffer(buffer, size, spec);
            ret = m_Writer->Write();
            m_Writer->Cleanup();

            return ret;
        }
    }

    return false;
}


/// MovieRenderer

MovieRenderer::MovieRenderer(const MEntry& entry, const EncodeSpec& spec)
    : m_Writer(std::move(Forge::Instance().GetMovieWriter(entry.Extension(), spec)))
    , m_MediaEntry(entry)
{
    if (m_Writer)
        m_Writer->Setup(m_MediaEntry.Fullpath());
}

MovieRenderer::~MovieRenderer()
{
    if (m_Writer)
        m_Writer->Cleanup();
}

bool MovieRenderer::AddBuffer(const void* buffer, std::size_t size, const InputSpec& spec)
{
    if (m_Writer)
        return m_Writer->AddBuffer(buffer, size, spec);

    return false;
}

bool MovieRenderer::Render()
{
    if (m_Writer)
        return m_Writer->Write();

    return false;
}

} // namespace Renderer

VOID_NAMESPACE_CLOSE
