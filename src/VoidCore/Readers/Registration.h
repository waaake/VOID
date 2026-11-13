// Copyright (c) 2025 waaake
// Licensed under the MIT License

#pragma once
/**
 * This file is temporary till the plugin system becomes enough
 * powerful as to dynamically load plugins with an entry point function
 * and all plugins related to images and movies can be excluded from this plugin
 * or maybe keeping a default one in place and others can be put externally
 */

/* Internal */
#include "Definition.h"
#include "FormatForge.h"
#include "OIIOReader.h"
#include "OpenEXRReader.h"
#include "FFmpegReader.h"

VOID_NAMESPACE_OPEN

namespace Internal {

void RegisterOIIOReader(Forge& registry)
{
    ReaderRegistry r;
    r.extensions = { "tga", "tiff", "dpx", "png", "JPG", "jpg", "jpeg", "JPEG" };
    r.reader = [](const std::string& path) -> std::unique_ptr<OIIOMediaReader> { return std::make_unique<OIIOMediaReader>(path); };

    registry.Register(r);
}

void RegisterOpenEXRReader(Forge& registry)
{
    ReaderRegistry r;
    r.extensions = { "exr" };
    r.reader = [](const std::string& path) -> std::unique_ptr<OpenEXRMediaReader> { return std::make_unique<OpenEXRMediaReader>(path); };

    registry.Register(r);
}

void RegisterFFmpegReader(Forge& registry)
{
    ReaderRegistry r;
    r.extensions = { "mp4", "mov", "MOV" };
    r.reader = [](const std::string& path) -> std::unique_ptr<FFmpegMediaReader> { return std::make_unique<FFmpegMediaReader>(path); };

    registry.Register(r);
}

} // namesapce Internal

/**
 * Register Inbuilt Media Readers
 * TODO: This needs to be change when we have a plugin manager to look at directories and load built
 * image readers from that. For now this needs to be done as a way to explicitly call this function
 * after VOID_LOG has been initialized if required
 */
void RegisterReaders()
{
    Forge& registry = Forge::Instance();
    
    Internal::RegisterOIIOReader(registry);
    Internal::RegisterOpenEXRReader(registry);
    Internal::RegisterFFmpegReader(registry);
}

VOID_NAMESPACE_CLOSE
