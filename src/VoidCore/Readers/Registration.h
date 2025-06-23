/* Internal */
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

        /* PNG */
        Forge::Instance().Register("png", []() { return std::make_unique<OIIOReader>(); });

        // /* JPG */
        // Forge::Instance().Register("JPG", []() { return std::make_unique<OIIOReader>(); });

        /* jpg */
        Forge::Instance().Register("jpg", []() { return std::make_unique<OIIOReader>(); });

        /* JPEG */
        Forge::Instance().Register("JPEG", []() { return std::make_unique<OIIOReader>(); });

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
