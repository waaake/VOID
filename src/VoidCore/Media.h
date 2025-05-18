#ifndef _VOID_MEDIA_H
#define _VOID_MEDIA_H

/* STD */
#include <filesystem>
#include <unordered_map>
#include <vector>

/* Internal */
#include "Definition.h"
#include "ImageData.h"
#include "Logging.h"

VOID_NAMESPACE_OPEN

class Frame
{
public:
    Frame();
    Frame(const std::string& path);

    virtual ~Frame();

    /* Reads the provided image frame */
    void Read(const std::string& path);

    /* Getters */
    inline std::string Path() const { return m_Path; }
    inline std::string Name() const { return m_Name; }
    inline std::string Extension() const { return m_Extension; }
    inline int Framenumber() const { return m_Framenum; }

    bool IsMovie() const;

    /* Returns the Pointer to the ImageData */
    VoidImageData* ImageData();

private: /* Members */
    std::string m_Path;
    std::string m_Name;
    std::string m_Extension;
    int m_Framenum;

    VoidImageData* m_ImageData;

    std::string m_ImageTypes[4] = {
        "png",
        "jpg",
        "jpeg",
        "exr",
    };

    std::string m_MovieTypes[4] = {
        "mov",
        "mp4",
        "mxf",
        "mkv"
    };

private: /* Methods */
    bool ValidFrame(const std::string& framestring) const;

};

class Media
{
public: /* Enums */
    enum class Type
    {
        UNDEFINED,
        IMAGE_SEQUENCE,
        SINGLE_FILE,
        NON_MEDIA
    };

public:
    Media();
    Media(const std::string& path);

    virtual ~Media();

    /*
     * Reads a given directory for files
     * If the file type (extension) is one of the valid types to be considered
     * as a media, then internal arrays are populated with frame data
     */
    void Read(const std::string& path);

    /* Getters */
    inline std::string Path() const { return m_Path; }
    inline std::string Name() const { return m_Name; }
    inline std::string Extension () const { return m_Extension; }
    inline Media::Type MediaType() const { return m_Type; }
    inline bool Valid() const { return m_Type != Type::NON_MEDIA; }

    int FirstFrame() const;
    int LastFrame() const;

private: /* Members */
    std::string m_Path;
    std::string m_Extension;
    std::string m_Name;
    int m_FirstFrame, m_LastFrame;

    Type m_Type;

    std::unordered_map<int, Frame> m_Mediaframes;
    std::vector<int> m_Framenumbers;

private: /* Methods */
    void ProcessMovie(const std::string& path);

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_H
