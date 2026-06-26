// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Exporter.h"
#include "MediaExporter.h"
#include "VoidUi/Engine/Globals.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

MediaExporter::MediaExporter(const SharedMediaClip& media, QWidget* parent)
    : ExportOptions(parent)
    , m_Media(media)
{
    Setup();
    connect(this, &ExportOptions::exported, this, &MediaExporter::Export);
}

MediaExporter::~MediaExporter()
{
}

void MediaExporter::Setup()
{
    setWindowTitle("Export Media");

    SetRange(m_Media->FirstFrame(), m_Media->LastFrame());
    SetRate(m_Media->Framerate());
}

void MediaExporter::Export()
{
    if (Validate())
    {
        // Resolution options provide a way for user to select half or quarter
        const int divisor = ScaleIndex() == 2 ? 4 : ScaleIndex() == 1 ? 2 : 1;
        int outwidth = m_Media->Width() / divisor;
        int outheight = m_Media->Height() / divisor;

        EncodeSpec spec(
            outwidth,
            outheight,
            m_Media->Channels(),
            Rate(),
            BufferType::Uint8,
            Codec(),
            false
        );
        UIGlobals::QueueTask(new ExportMediaFramesTask(m_Media, m_Descriptor, spec, Range(), Colorspace().toStdString()));
        close();
    }
}

bool MediaExporter::Validate()
{
    ClearLog();

    // TODO: Write the actual validator mechanism
    if (!m_Descriptor.entry.Valid())
    {
        Log("Missing export path.", TaskLog::Level::ErrorLog);
        return false;
    }

    MFrameRange range = Range();
    if (range.duration < 0)
    {
        Log("Invalid start and end frame", TaskLog::Level::ErrorLog);
        return false;
    }

    if (range.startframe < m_Media->FirstFrame() || range.endframe > m_Media->LastFrame())
    {
        Log(
            QString("Provided range is out of bounds of the selected media frame range, %1 - %2")
                .arg(m_Media->FirstFrame())
                .arg(m_Media->LastFrame()),
            TaskLog::Level::ErrorLog
        );
        return false;
    }

    // Just to let the user know that they have it set but not change anything, maybe they forgot?
    if (RangeOverridden() && range.startframe == m_Media->FirstFrame() && range.endframe == m_Media->LastFrame())
        Log("Media range is overridden but not changed", TaskLog::Level::WarningLog);

    if (m_Descriptor.type == WriterType::Movie &&
            (Codec() == MovieCodec::DNXHD || Codec() == MovieCodec::PRORES) &&
                m_Descriptor.entry.Extension() != "mov")
    {
        Log("Selected Movie codec can only be exported as a quicktime with .mov extension", TaskLog::Level::ErrorLog);
        return false;
    }

    if (m_Descriptor.type == WriterType::Movie && Rate() < 1.f)
    {
        Log("Invalid output media framerate", TaskLog::Level::ErrorLog);
        return false;
    }

    if (m_Descriptor.type == WriterType::Image && !m_Descriptor.entry.Templated())
    {
        Log("Provided path is not templated .i.e. does not contain tokens for replacement '####'", TaskLog::Level::ErrorLog);
        return false;
    }

    Log(
        m_Descriptor.type == WriterType::Movie ? "Processing as movie" : "Processing as image sequence",
        TaskLog::Level::InfoLog
    );
    return true;
}

VOID_NAMESPACE_CLOSE
