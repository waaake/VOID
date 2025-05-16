/* STD */
#include <filesystem>
#include <chrono>

/* Internal */
#include "ImageBuffer.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

VoidImage::VoidImage(const std::string& basePath, const std::string& name, const std::string& extension, int frame)
    : m_BasePath(basePath)
    , m_Name(name)
    , m_Extension(extension)
    , m_Frame(frame)
    , m_FullPath("")
{
    m_ImageData = new VoidImageData;
}

VoidImage::VoidImage(const std::string& path)
    : VoidImage("", "", "", -1)
{
    m_FullPath = path;
    ConstructFromPath(path);
}

VoidImage::~VoidImage()
{
    // delete m_ImageData;
}

bool VoidImage::ValidFrame(const std::string& frame) const
{
    if (frame.empty())
        return false;
    
    for (char c: frame)
    {
        if (!std::isdigit(c))
            return false;
    }

    /* Valid frame */
    return true;
}

void VoidImage::ConstructFromPath(const std::string& path)
{
    std::filesystem::path filepath(path);

    m_BasePath = filepath.parent_path().string();

    std::string filename = filepath.filename().string();

    size_t lastDot = filename.find_last_of(".");
    std::string remaining = filename.substr(0, lastDot);

    m_Extension = filename.substr(lastDot + 1);

    lastDot = remaining.find_last_of(".");
    std::string framestring = remaining.substr(lastDot + 1);

    /* Image is a sequence or atleast follows a sequential naming convention */
    if (ValidFrame(framestring))
    {
        m_Frame = std::stoi(framestring);
        m_Name = remaining.substr(0, lastDot);
    }
    else // In case we don't have an image sequence, just a standard image
    {
        m_Name = remaining;
    }
}

std::string VoidImage::FullPath()
{
    if (m_FullPath.empty())
    {
        std::filesystem::path p = m_BasePath;
        
        /* Create a fullname for appending with the basepath */
        std::string fullname;

        /* We have a valid frame number */
        if (m_Frame != -1)
        {
            fullname = m_Name + "." + std::to_string(m_Frame) + "." + m_Extension;
        }
        else
        {
            fullname = m_Name + "." + m_Extension;
        }

        p.append(fullname);

        m_FullPath = p.string();
    }
    
    return m_FullPath;
}

VoidImageData* VoidImage::GetImageData()
{
    /*
        * If the frame data has not yet been fetched
        * Read the frame data and return the pointer to the data
        */
    if (m_ImageData->Empty())
        m_ImageData->Read(m_FullPath);

    return m_ImageData;
}

VoidImageSequence::VoidImageSequence()
    : m_BasePath("")
{

}

VoidImageSequence::VoidImageSequence(const std::string& path)
{
    Read(path);
}

// VoidImageSequence::VoidImageSequence(const VoidImageSequence& other)
// {

//     // std::unordered_map<int, VoidImage> m_Images;
//     // std::string m_BasePath;
//     // std::vector<int> m_Frames;

//     for (std::unordered_map<int, VoidImage>::const_iterator it = other.m_Images.begin(); it != other.m_Images.end(); ++it)
//     {
//         m_Images[it->first] = it->second;
//     }

//     /* Since we know the size of the vector use it to reserve */
//     m_Frames.reserve(other.m_Frames.size());

//     for (int frame: other.m_Frames)
//     {
//         m_Frames.emplace_back(frame);
//     }

//     /* Update the base path */
//     m_BasePath = other.m_BasePath;
// }

void VoidImageSequence::Read(const std::string& path)
{
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();

    /* Update the base path */
    m_BasePath = path;

    try
    {
        for (std::filesystem::directory_entry entry: std::filesystem::directory_iterator(path))
        {
            VoidImage image = entry.path().string();
            // Map against the frame
            int frame = image.GetFrame();
            m_Images[frame] = image;
            m_Frames.push_back(frame);
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
    }

    std::chrono::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    VOID_LOG_INFO("Time Taken to Load : {0}", duration.count());
}

int VoidImageSequence::FirstFrame() const
{
    std::vector<int>::const_iterator fIt = std::min_element(m_Frames.begin(), m_Frames.end());

    if (fIt != m_Frames.end())
    {
        return *fIt;
    }

    return -1;
}

int VoidImageSequence::LastFrame() const
{
    std::vector<int>::const_iterator fIt = std::max_element(m_Frames.begin(), m_Frames.end());

    if (fIt != m_Frames.end())
    {
        return *fIt;
    }

    return -1;
}

VOID_NAMESPACE_CLOSE
