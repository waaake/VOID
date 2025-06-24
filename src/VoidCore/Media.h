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
#include "MediaFilesystem.h"

VOID_NAMESPACE_OPEN

class VOID_API Frame
{
public:
    Frame();
    explicit Frame(const MEntry& e);

    ~Frame();

    void Set(const MEntry& e);

    /* Getters */
    inline std::string Path() const { return m_MediaEntry.Fullpath(); }
    inline std::string Name() const { return m_MediaEntry.Name(); }
    inline std::string Extension() const { return m_MediaEntry.Extension(); }
    inline int Framenumber() const { return m_MediaEntry.Framenumber(); }

    /* Returns the Pointer to the ImageData */
    VoidImageData* ImageData();

    /* Frame Caches */
    void Cache();
    void ClearCache();

private: /* Members */
    MEntry m_MediaEntry;
    VoidImageData* m_ImageData;

};

class VOID_API Media
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
    Media(const MediaStruct& mstruct);

    virtual ~Media();

    /**
     * Reads a MediaStructure to update the internals based on the
     * entries in the structure
     */
    void Read(const MediaStruct& mstruct);

    /* Getters */
    inline std::string Path() const { return m_MediaStruct.Basepath(); }
    inline std::string Name() const { return m_MediaStruct.Name(); }
    inline std::string Extension() const { return m_MediaStruct.Extension(); }

    inline Media::Type MediaType() const { return m_Type; }

    // inline bool Valid() const { return m_Type != Type::NON_MEDIA; }

    inline int FirstFrame() const { return m_FirstFrame; }
    inline int LastFrame() const { return m_LastFrame; }

    inline int Duration() const { return (LastFrame() - FirstFrame()) + 1; }

    /*
     * Returns whether a given frame falls in the range of Media
     * i.e. between the first and the last frame of media
     * Any frame missing does not matter as this method only returns whether a frame is in the range or not
     */
    [[nodiscard]] inline bool HasFrame(const int frame) const { return frame >= m_FirstFrame && frame <= m_LastFrame; }

    /* 
     * Returns whether a given frame is available to read
     * There could be a scenario where the given frame is in the range of first - last but is not available
     * and is referred to as the missing frame.
     */
    [[nodiscard]] inline bool Contains(const int frame) const { return m_Mediaframes.find(frame) != m_Mediaframes.end(); }

    /*
     * Based on the available frames, returns the frame which is just lower than the provided frame
     * This is used when the current frame is not available but we want the neartest frame to be used in it's place
     */
    int NearestFrame(const int frame) const;

    Frame GetFrame(const int frame) const { return m_Mediaframes.at(frame); }

    Frame FirstFrameData() const { return m_Mediaframes.at(FirstFrame()); }
    Frame LastFrameData() const { return m_Mediaframes.at(LastFrame()); }

    inline VoidImageData* Image(const int frame) { return m_Mediaframes.at(frame).ImageData(); }

    VoidImageData* FirstImage() { return Image(FirstFrame()); }
    VoidImageData* LastImage() { return Image(LastFrame()); }

    inline double Framerate() const { return 24.0; }
    inline bool Empty() const { return m_Mediaframes.empty(); }
    /*
     * A Media can be considered invalid if it is empty
     * Any valid media will have atleast one frame
     */
    inline bool Valid() const { return !Empty(); }

    inline void Clear()
    {
        /* Clear underlying structs */
        m_Framenumbers.clear();
        m_Mediaframes.clear();
    }

    /*
     * Caches all frames of the Media onto memory
     * This method is CPU intensive function and should be called from a separate thread
     * to allow this function to run parallelly to other elements
     */
    void Cache();

    /*
     * Clears the cache for all the frames of the Media
     */
    void ClearCache();

    /* Stops caching if the cache process is ongoing */
    void StopCaching();

    /* Returns whether the Cache process is ongoing */
    inline bool Caching() const { return m_StopCaching; }

    /* Allow iterating over the Media frames */
    inline std::unordered_map<int, Frame>::iterator begin() { return m_Mediaframes.begin(); }
    inline std::unordered_map<int, Frame>::iterator end() { return m_Mediaframes.end(); }

private: /* Members */
    /**
     * The Media structure for the Media
     */
    MediaStruct m_MediaStruct;

    int m_FirstFrame, m_LastFrame;

    Type m_Type;

    /* State determining that caching is currently ongoing */
    bool m_Caching;
    /*
     * This variable controls when the caching operation needs to be stopped
     * If set to True and if the caching process was active
     */
    bool m_StopCaching;

    /* Arrays to hold the media Frames for the type of media */
    std::unordered_map<int, Frame> m_Mediaframes;
    /* Array to hold the frame numbers for the frames which have been read */
    std::vector<int> m_Framenumbers;

private: /* Methods */
    void ProcessMovie(const MediaStruct& mstruct);

    /* Updates the internal range based on the read frames */
    void UpdateRange();

};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_H
