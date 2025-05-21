#ifndef _VOID_IMAGE_DATA_H
#define _VOID_IMAGE_DATA_H

/* STD*/
#include <string>

/* extern */
#include "../extern/stb_image.h"

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

class VoidImageData
{
private:
    /* Image data */
    int m_Width, m_Height;
    int m_Channels;

    unsigned char* m_Data;

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

    int Width() const { return m_Width; }
    int Height() const { return m_Height; }

    int Channels() const { return m_Channels; }
    
    unsigned char* Data() const { return m_Data; }

    bool Empty() const { return !(m_Data); }

    void Free();
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_DATA_H
