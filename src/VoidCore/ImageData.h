#ifndef _VOID_IMAGE_DATA_H
#define _VOID_IMAGE_DATA_H

/* STD*/
#include <string>
#include <vector>

/* OpenImageIO */
#include <OpenImageIO/imageio.h>

/* Internal */
#include "Definition.h"
#include "Logging.h"

VOID_NAMESPACE_OPEN

class VoidImageData
{
private: /* Members */
    /* Image Specification Data */
    int m_Width, m_Height;
    int m_Channels;

    /* Indicating that the Image data has not yet been read */
    bool m_Empty;

    std::vector<unsigned char> m_Pixels;

    /* The frame path */
    std::string m_Filepath;

public:
    VoidImageData();
    VoidImageData(const std::string& path);

    virtual ~VoidImageData();

    /* Accessors */
    /*
     * Reads the provided image
     * Loads the image data onto the memory
     */
    void Read(const std::string& path);

    inline int Width() const { return m_Width; }
    inline int Height() const { return m_Height; }
    inline int Channels() const { return m_Channels; }

    inline const unsigned char* Data() const { return m_Pixels.data(); }

    inline bool Empty() const { return m_Empty; }
    inline void Free() { m_Pixels.clear(); };
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_DATA_H
