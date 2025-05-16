#ifndef _VOID_IMAGE_BUFFER_H
#define _VOID_IMAGE_BUFFER_H

/* STD */
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>

/* Internal */
#include "Definition.h"
#include "VoidCore/ImageData.h"

VOID_NAMESPACE_OPEN

class VoidImage
{
private:
    std::string m_BasePath;
    std::string m_Name;
    std::string m_Extension;

    std::string m_FullPath;
    
    int m_Frame;

    VoidImageData* m_ImageData;

protected:
    [[nodiscard]] bool ValidFrame(const std::string& frame) const;

public:
    VoidImage() {};
    VoidImage(const std::string& basePath, const std::string& name, const std::string& extension, int frame);
    VoidImage(const std::string& path);

    virtual ~VoidImage();

    std::string FullPath();
    std::string BasePath() const { return m_BasePath; }
    std::string Name() const { return m_Name; }
    std::string Extension() const { return m_Extension; }
    int GetFrame() const { return m_Frame; }

    VoidImageData* GetImageData();

    void ConstructFromPath(const std::string& path);
    void UpdateImage();
};

class VoidImageSequence
{
private:
    std::unordered_map<int, VoidImage> m_Images;
    std::string m_BasePath;
    std::vector<int> m_Frames;

public:
    VoidImageSequence();
    VoidImageSequence(const std::string& path);

    /* Copy */
    // VoidImageSequence(const VoidImageSequence& other);

    /* Destroy */
    virtual ~VoidImageSequence() {} ;

    VoidImage GetImage(int frame) const { return m_Images.at(frame); }
    VoidImage FirstImage() const { return m_Images.at(FirstFrame()); }
    VoidImage LastImage() const { return m_Images.at(LastFrame()); }

    std::string Name() const { return m_Images.at(FirstFrame()).Name(); }
    std::string Type() const { return m_Images.at(FirstFrame()).Extension(); }
    /*
     * Returns the rate at which the media can be played
     * Would be read from the metadata of single playable files
     * Else would mostly be dependent on the rate at which the media is being played
     */
    double Framerate() const { return 24.0; }

    bool Empty() const { return m_Frames.empty(); }

    void Read(const std::string& path);

    int FirstFrame() const;
    int LastFrame() const;

    std::string BasePath() const { return m_BasePath; }
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_IMAGE_BUFFER_H
