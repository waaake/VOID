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

VOID_NAMESPACE_OPEN

/* Registration {{{ */
struct OIIOReaderPlugin
{
    OIIOReaderPlugin()
    {
        /**
         * OIIO Image Reader supports a Bunch of Image formats
         * Adding all of them
         */

        /**
         * For now Going with all types of formats separately like "jpg" and "JPG" differ in cases
         * We could go for a tolower but the case might have unnecessary copies going for each extension
         * and for each of the frame that's from the source which looks too expensive at the moment
         * 
         * TODO: have a thought about this again at a later stage to see if we can do something better than this
         */

        /* PNG */
        Forge::Instance().RegisterImageReader("png", []() { return std::make_unique<OIIOPixReader>(); });

        /* JPG */
        Forge::Instance().RegisterImageReader("JPG", []() { return std::make_unique<OIIOPixReader>(); });

        /* jpg */
        Forge::Instance().RegisterImageReader("jpg", []() { return std::make_unique<OIIOPixReader>(); });

        /* JPEG */
        Forge::Instance().RegisterImageReader("JPEG", []() { return std::make_unique<OIIOPixReader>(); });

    }
};

/**
 * Register Inbuilt Media Readers
 * TODO: This needs to be change when we have a plugin manager to look at directories and load built
 * image readers from that. For now this needs to be done as a way to explicitly call this function
 * after VOID_LOG has been initialized if required
 */
VOID_API void RegisterReaders()
{
    /* Register OIIO Reader */
    OIIOReaderPlugin o;
}

VOID_NAMESPACE_CLOSE
