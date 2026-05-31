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

ImageRenderer::ImageRenderer(const MEntry& entry, int width, int height, int channels)
    : m_Writer(std::move(Forge::Instance().GetImageWriter(entry.Extension(), width, height, channels, WriterType::Image)))
    , m_MediaEntry(entry)
{
}

ImageRenderer::~ImageRenderer()
{
    // if (m_Writer)
    //     m_Writer->Cleanup();
}

bool ImageRenderer::AddBuffer(const void* buffer, std::size_t size, const BufferType& type)
{
    // if (m_Writer)
    //     m_Writer->AddBuffer(buffer, size, type);
    if (m_Writer)
    {
        if (m_Writer->Setup(m_MediaEntry.Fullpath()))
        {
            m_Writer->AddBuffer(buffer, size, type);
            return true;
        }
    }

    return false;
}

bool ImageRenderer::AddBuffer(v_frame_t frame, const void* buffer, std::size_t size, const BufferType& type)
{
    // if (m_Writer)
    //     m_Writer->AddBuffer(buffer, size, type);
    if (m_Writer)
    {
        if (m_Writer->Setup(m_MediaEntry.ResolvedPath(frame)))
        {
            m_Writer->AddBuffer(buffer, size, type);
            return true;
        }
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

bool ImageRenderer::Render(const void* buffer, std::size_t size, const BufferType& type)
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("ImageRenderer::Render");
    if (m_Writer)
    {
        if (m_Writer->Setup(m_MediaEntry.Fullpath()))
        {
            m_Writer->AddBuffer(buffer, size, type);
            m_Writer->Write();
            m_Writer->Cleanup();

            return true;
        }
    }

    return false;
}

bool ImageRenderer::Render(v_frame_t frame, const void* buffer, std::size_t size, const BufferType& type)
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("ImageRenderer::Render");
    if (m_Writer)
    {
        if (m_Writer->Setup(m_MediaEntry.ResolvedPath(frame)))
        {
            m_Writer->AddBuffer(buffer, size, type);
            m_Writer->Write();
            m_Writer->Cleanup();

            return true;
        }
    }

    return false;
}


/// MovieRenderer

MovieRenderer::MovieRenderer(const MEntry& entry, int width, int height, int channels)
    : m_Writer(std::move(Forge::Instance().GetMovieWriter(entry.Extension(), width, height, channels, WriterType::Movie)))
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

bool MovieRenderer::AddBuffer(const void* buffer, std::size_t size, const BufferType& type)
{
    if (m_Writer)
    {
        m_Writer->AddBuffer(buffer, size, type);
        return true;
    }

    return false;
}

bool MovieRenderer::Render()
{
    if (m_Writer)
        return m_Writer->Write();

    return false;
}


bool RenderImage(const MEntry& e, int w, int h, int ch, const void* buf, std::size_t size, const BufferType& type)
{
    // VOID_LOG_INFO("RenderImage::Path: {0}, extension: {1}", e.Fullpath(), e.Extension());
    // VOID_LOG_INFO("Image Specs: {0}x{1}, channels: {2}, size: {3}", w, h, ch, size);
    
    Tools::VoidProfiler<std::chrono::duration<double>> p("Renderer::RenderImage");
    // auto writer = std::move(Forge::Instance().GetImageWriter(e.Extension(), w, h, ch, WriterType::Image));
    if (auto writer = std::move(Forge::Instance().GetImageWriter(e.Extension(), w, h, ch, WriterType::Image)))
    {
        // VOID_LOG_INFO("Writer found for extension: {}", e.Extension());
        // writer->AddBuffer(buf, size, type);
        // return writer->Write(e.Fullpath());

        if (writer->Setup(e.Fullpath()))
        {
            writer->AddBuffer(buf, size, type);
            writer->Write();
            writer->Cleanup();

            return true;
        }
    }

    return false;
}

bool RenderImage(
    const MEntry& e, v_frame_t f, int w, int h, int ch, const void* buf, std::size_t size, const BufferType& type
)
{
    // This method does not concern whether the entry is templated correctly or not
    // That's something which gets handled at the calling function level
    // Assuming the entry is correct and that the resolved path with the frame returns correct path to the numbered frame
    
    Tools::VoidProfiler<std::chrono::duration<double>> p("Renderer::RenderImage");
    // auto writer = std::move(Forge::Instance().GetImageWriter(entry.Extension(), width, height, channels, WriterType::Image));
    if (auto writer = std::move(Forge::Instance().GetImageWriter(e.Extension(), w, h, ch, WriterType::Image)))
    {
        // VOID_LOG_INFO("Writer found for extension: {}", e.Extension());
        // writer->AddBuffer(buf, size, type);
        // return writer->Write(e.ResolvedPath(f));
        if (writer->Setup(e.ResolvedPath(f)))
        {
            writer->AddBuffer(buf, size, type);

            writer->Write();
            writer->Cleanup();
            return true;
        }
    }

    return false;
}

} // namespace Renderer

VOID_NAMESPACE_CLOSE
