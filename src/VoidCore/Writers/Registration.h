// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Definition.h"
#include "FormatForge.h"
#include "OIIOWriter.h"
#include "FFmpegWriter.h"

VOID_NAMESPACE_OPEN

namespace Internal {

    bool RegisterOpenImageIOWriter()
    {
        WriterRegistry f;
        f.name = "OIIO Writer";
        f.extensions = { "png", "jpg" };
        f.type = WriterType::Image;
        f.writer = [](const EncodeSpec& spec) -> std::unique_ptr<OIIOWriter>
                        {
                            return std::make_unique<OIIOWriter>(spec);
                        };

        return Forge::Instance().Register(f);
    }

    bool RegisterFFmpegWriter()
    {
        WriterRegistry f;
        f.name = "FFmpeg Writer";
        f.extensions = { "mov", "mp4" };
        f.type = WriterType::Movie;
        f.writer = [](const EncodeSpec& spec) -> std::unique_ptr<FFmpegWriter>
                        {
                            return std::make_unique<FFmpegWriter>(spec);
                        };

        return Forge::Instance().Register(f);
    }

} // namespace Internal

void RegisterWriters()
{
    Internal::RegisterOpenImageIOWriter();
    Internal::RegisterFFmpegWriter();
}

VOID_NAMESPACE_CLOSE
