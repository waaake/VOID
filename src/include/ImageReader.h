#ifndef _VOID_IMAGE_READER_H
#define _VOID_IMAGE_READER_H

/* STD */
#include <memory>

/* Internal */
#include "Definition.h"
#include "PixBlock.h"

VOID_NAMESPACE_OPEN

typedef std::shared_ptr<PixBlock> SharedPixBlock;

class ImageReader
{
public:
    virtual ~ImageReader() {}

    /**
     * The Main Read mechanism
     * Takes in a path of an image
     * Returns (After successful read) Shared Pointer to the PixelBlock
     * holding all of the image information
     * If the image was not read successfully a null pointer is returned instead
     */
    virtual SharedPixBlock Read(const std::string& path) = 0;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_READER_H
