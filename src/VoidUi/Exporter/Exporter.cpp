// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Exporter.h"
#include "VoidCore/ColorProcessor.h"
#include "VoidUi/Player/Player.h"
#include "VoidCore/Media/Renderer.h"

VOID_NAMESPACE_OPEN

ExportAnnotatedFramesTask::ExportAnnotatedFramesTask(const MediaExportDescriptor& descriptor, Player* player)
    : Task("Export Annotated Frames")
    , m_Player(player)
    , m_Descriptor(descriptor)
{
}

bool ExportAnnotatedFramesTask::Work()
{
    const auto& annotations = m_Player->m_ActiveViewBuffer->Annotations();
    if (annotations.empty())
        return false;

    SetMax(static_cast<int>(annotations.size()));

    int count = 1;


    if (m_Descriptor.type == WriterType::Image && m_Descriptor.entry.Templated())
    {
        const Renderer::RenderData r = m_Player->m_Renderer->FrameBuffer();
        Renderer::ImageRenderer ir(m_Descriptor.entry, {r.width, r.height, r.channels, r.type});

        for (auto& [frame, _] : annotations)
        {
            if (auto data = m_Player->m_ActiveViewBuffer->MData(frame))
            {
                m_Player->m_Renderer->Render(data.image, data.annotation);
                const Renderer::RenderData r = m_Player->m_Renderer->FrameBuffer();

                if (!ir.Render(frame, r.pixels.data(), r.Size(), {r.width, r.height, r.channels, r.type}))
                {
                    // ErrorMessageBox box("There was an error in exporting media.", "Error", this);
                    // box.exec();
                    return false;
                }

                SetProgress(count);
                count++;
            }
        }

        // Render back the current frame
        // This is obviously temporary till we have an offscreen renderer available for using offscreen framebuffer
        m_Player->Refresh();

        // InfoMessageBox box("Annotated frames have been exported.", "Success", this);
        // box.exec();
        return true;
    }
    else if (m_Descriptor.type == WriterType::Movie)
    {
        const Renderer::RenderData r = m_Player->m_Renderer->FrameBuffer();
        Renderer::MovieRenderer mr(m_Descriptor.entry, {r.width, r.height, r.channels, r.type});

        for (auto& [frame, _] : annotations)
        {
            if (auto data = m_Player->m_ActiveViewBuffer->MData(frame))
            {
                m_Player->m_Renderer->Render(data.image, data.annotation);
                const Renderer::RenderData r = m_Player->m_Renderer->FrameBuffer();
                if (!mr.AddBuffer(r.pixels.data(), r.Size(), {r.width, r.height, r.channels, r.type}))
                {
                    // ErrorMessageBox box("There was an error in exporting media.", "Error", this);
                    // box.exec();
                    return false;
                }

                SetProgress(count);
                count++;
            }
        }

        mr.Render();

        // Render back the current frame
        // This is obviously temporary till we have an offscreen renderer available for using offscreen framebuffer
        m_Player->Refresh();

        // InfoMessageBox box("Annotated frames have been exported.", "Success", this);
        // box.exec();
        return true;
    }

    return false;
}

/// ExportMediaFramesTask

ExportMediaFramesTask::ExportMediaFramesTask(const SharedMediaClip& media, const MediaExportDescriptor& descriptor, const EncodeSpec& spec, const MFrameRange& range, const std::string& colorspace)
    : Task("Transcode Media")
    , m_Media(media)
    , m_Descriptor(descriptor)
    , m_Spec(spec)
    , m_Range(range)
    , m_Colorspace(colorspace)
{
}

void ExportMediaFramesTask::ProcessImage(const void* pixels, int width, int height, int channels, const ColorSpace& incolorspace)
{
    const unsigned char* cpixels = static_cast<const unsigned char*>(pixels);

    for (int i = 0; i < (width * height * channels); ++i)
        m_Pixels[i] = cpixels[i] / 255.0f;

    // Image processing
    ColorProcessor::Instance().ProcessImage(m_Pixels.data(), width, height, channels, incolorspace, m_Colorspace);
}

bool ExportMediaFramesTask::Work()
{
    if (SharedMediaClip media = m_Media.lock())
    {
        int count = 0;
        SetMax(m_Range.duration);

        // All the media frames would have the same size
        m_Pixels.resize(media->FirstImage()->FrameSize());

        if (m_Descriptor.type == WriterType::Image)
        {
            if (!m_Descriptor.entry.Templated())
            {
                Log(
                    QString("Provied filepath to render is not templated (i.e. filename.####.ext). %1")
                        .arg(m_Descriptor.entry.Fullpath().c_str()),
                    TaskLog::Level::ErrorLog
                );
                return false;
            }

            Log(
                QString("Starting Image render. Output will be saved to: %1, Range: %2 - %3")
                    .arg(m_Descriptor.entry.Fullpath().c_str())
                    .arg(m_Range.startframe)
                    .arg(m_Range.endframe),
                    TaskLog::Level::InfoLog
                );

            Renderer::ImageRenderer ir(m_Descriptor.entry, m_Spec);

            for (int i = m_Range.startframe; i <= m_Range.endframe; ++i)
            {
                // Check for whether the task was cancelled
                if (Cancelled())
                    return false;

                if (!media->Contains(i))
                {
                    Log(QString("Media does not contain frame %1 for render. Skipping.").arg(i), TaskLog::Level::WarningLog);
                    continue;
                }

                SharedPixels image = media->Image(i);

                /// Colorspace processor
                ProcessImage(image->Pixels(), image->Width(), image->Height(), image->Channels(), image->InputColorSpace());
                if (!ir.Render(i, m_Pixels.data(), image->FrameSize(), {image->Width(), image->Height(), image->Channels(), BufferType::Float}))
                {
                    Log(QString("Unable to render current frame: %1").arg(i), TaskLog::Level::ErrorLog);
                    return false;
                }
                // Don't want to take up additional memory, so clear as we go...
                // We definitely need a better way to handle this
                // Other way could be to read through the ViewerBuffer, but that's something will eventually come
                image->Clear();

                count++;
                SetProgress(count);
                Log(
                    QString("ImageRenderer: Frame %1 of %2 rendered").arg(i).arg(m_Range.endframe),
                    TaskLog::Level::InfoLog
                );
            }

            Log("Image render completed", TaskLog::Level::InfoLog);
            return true;
        }
        else if (m_Descriptor.type == WriterType::Movie)
        {
            Log(
                QString("Starting Movie render. Output will be saved to: %1, Range: %2 - %3")
                    .arg(m_Descriptor.entry.Fullpath().c_str())
                    .arg(m_Range.startframe)
                    .arg(m_Range.endframe),
                TaskLog::Level::InfoLog
            );

            Renderer::MovieRenderer mr(m_Descriptor.entry, m_Spec);

            for (int i = m_Range.startframe; i <= m_Range.endframe; ++i)
            {
                // Check for whether the task was cancelled
                if (Cancelled())
                    return false;

                if (!media->Contains(i))
                {
                    Log(QString("Media does not contain frame %1 for render. Skipping.").arg(i), TaskLog::Level::WarningLog);
                    continue;
                }

                SharedPixels image = media->Image(i);

                /// Colorspace processor
                ProcessImage(image->Pixels(), image->Width(), image->Height(), image->Channels(), image->InputColorSpace());
                if (!mr.AddBuffer(m_Pixels.data(), image->FrameSize(), {image->Width(), image->Height(), image->Channels(), BufferType::Float}))
                {
                    Log(QString("Unable to buffer current frame: %1").arg(i), TaskLog::Level::ErrorLog);
                    return false;
                }
                // Don't want to take up additional memory, so clear as we go...
                // We definitely need a better way to handle this
                // Other way could be to read through the ViewerBuffer, but that's something will eventually come
                image->Clear();

                count++;
                SetProgress(count);
                Log(
                    QString("MovieRenderer: Frame %1 of %2 buffered").arg(i).arg(m_Range.endframe),
                    TaskLog::Level::InfoLog
                );
            }

            mr.Render();

            Log("Movie render completed", TaskLog::Level::InfoLog);
            return true;
        }

        Log(
            QString("Unable to determine the path/extension for render correctly. %1")
                .arg(m_Descriptor.entry.Fullpath().c_str()),
            TaskLog::Level::ErrorLog
        );
        return false;
    }

    Log("Unable to access media.", TaskLog::Level::ErrorLog);
    return false;
}

VOID_NAMESPACE_CLOSE
