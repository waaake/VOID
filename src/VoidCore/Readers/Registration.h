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
#include "TurboJpegReader.h"

VOID_NAMESPACE_OPEN

/* Registration {{{ */
struct OIIOReaderPlugin
{
    OIIOReaderPlugin()
    {
        /**
         * For now Going with all types of formats separately like "jpg" and "JPG" differ in cases
         * We could go for a tolower but the case might have unnecessary copies going for each extension
         * and for each of the frame that's from the source which looks too expensive at the moment
         */ 
        /* Image Registry */
        FormatRegistry<PixForge> f;

        f.name = "OIIO Reader";
        /**
         * OIIO supports multiple formats
         * internally reading from original library functions
         * Using this as the base Image Reader
         * If at any point we want to have a dedicated original library based reader, that could also be implemented
         * as a custom plugin
         */
        f.extensions = { "tga", "tiff", "dpx", "png" };
        /* OpenImageIO Reader */
        f.reader = [](const std::string& path, v_frame_t framenumber = 0) -> std::unique_ptr<OIIOPixReader> { return std::make_unique<OIIOPixReader>(path, framenumber); };

        /* Register Plugin */
        Forge::Instance().Register(f);
    }
};

struct OpenEXRReaderPlugin
{
    OpenEXRReaderPlugin()
    {
        /* exr */
        // Forge::Instance().RegisterImageReader("exr", []() { return std::make_unique<OpenEXRReader>(); });

        /* Image Registry */
        FormatRegistry<PixForge> f;

        f.name = "EXR Reader";
        f.extensions = { "exr" };
        /* Open EXR Reader*/
        f.reader = [](const std::string& path, v_frame_t framenumber = 0) -> std::unique_ptr<OpenEXRReader> { return std::make_unique<OpenEXRReader>(path, framenumber); };

        /* Register Plugin */
        Forge::Instance().Register(f);
    }
};

struct FFmpegReaderPlugin
{
    FFmpegReaderPlugin()
    {
        /* Movie Registry */
        FormatRegistry<MPixForge> f;

        f.name = "FFmpeg Reader";
        f.extensions = { "mp4", "mov" };
        /* FFmpeg Reader */
        f.reader = [](const std::string& path, v_frame_t framenumber = 0) -> std::unique_ptr<FFmpegPixReader> { return std::make_unique<FFmpegPixReader>(path, framenumber); };

        /* Register Plugin */
        Forge::Instance().Register(f);
    }
};

struct TurboJpegReaderPlugin
{
    TurboJpegReaderPlugin()
    {
        /**
         * For now Going with all types of formats separately like "jpg" and "JPG" differ in cases
         * We could go for a tolower but the case might have unnecessary copies going for each extension
         * and for each of the frame that's from the source which looks too expensive at the moment
         */ 
        /* Image Registry */
        FormatRegistry<PixForge> f;

        f.name = "Turbo JPEG Reader";
        /**
         * Support all known extension types for Lossy jpeg
         */
        f.extensions = { "JPG", "jpg", "jpeg", "JPEG" };
        /* Turbo JPEG Reader */
        f.reader = [](const std::string& path, v_frame_t framenumber = 0) -> std::unique_ptr<TurboJpegReader> { return std::make_unique<TurboJpegReader>(path, framenumber); };

        /* Register Plugin */
        Forge::Instance().Register(f);
    }
};

/**
 * Register Inbuilt Media Readers
 * TODO: This needs to be change when we have a plugin manager to look at directories and load built
 * image readers from that. For now this needs to be done as a way to explicitly call this function
 * after VOID_LOG has been initialized if required
 */
void RegisterReaders()
{
    /* Register OIIO Reader */
    OIIOReaderPlugin o;

    /* Register OpenEXR Reader */
    OpenEXRReaderPlugin openx;

    /* FFmpeg Reader */
    FFmpegReaderPlugin f;

    /* Turbo JPEG Reader */
    TurboJpegReaderPlugin t;
}

VOID_NAMESPACE_CLOSE
