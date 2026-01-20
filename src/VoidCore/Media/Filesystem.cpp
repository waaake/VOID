// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <algorithm>
#include <iomanip>
#include <regex>
#include <sstream>
#include <unordered_set>

/* Internal */
#include "Filesystem.h"
#include "FormatForge.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

/* Media Struct {{{ */

MediaStruct::MediaStruct()
    : m_Start(-1)
    , m_End(-1)
    , m_MediaType(MediaType::NonMedia)
{
}

MediaStruct::MediaStruct(Frame& frame, const MediaType& type)
    : m_Start(-1)
    , m_End(-1)
{
    /* Setup Media */
    Reset(frame, type);
}

MediaStruct::~MediaStruct()
{
}

MediaStruct::MediaStruct(const std::string& basepath, const std::string& name, const std::string& extension)
    : m_Start(-1)
    , m_End(-1)
{
    /* This is a Single file */
    Frame f(basepath, name, extension, 0, 0, true);
    Reset(f, MHelper::GetMediaType(f));
}

MediaStruct::MediaStruct(const std::string& basepath,
                    const std::string& name,
                    const std::string& extension,
                    v_frame_t start,
                    v_frame_t end,
                    unsigned int padding
                )
{
    /* Clear underlying structs */
    if (!Empty())
        Clear();

    m_Frames.reserve(end - start + 1);

    for (v_frame_t frame = start; frame <= end; ++frame)
    {
        m_Entries[frame] = std::move(Frame(basepath, name, extension, frame, padding, false));
        m_Frames.emplace_back(frame);
    }

    /* Update the media type */
    m_MediaType = MediaType::Image;
    UpdateRange();
}

MediaStruct::MediaStruct(const std::string& basepath,
                    const std::string& name,
                    const std::string& extension,
                    v_frame_t start,
                    v_frame_t end,
                    unsigned int padding,
                    const std::vector<v_frame_t>& missing
                )
{
    /* Clear underlying structs */
    if (!Empty())
        Clear();

    m_Frames.reserve((end - start + 1) - missing.size());

    std::unordered_set<v_frame_t> missingSet(missing.begin(), missing.end());

    for (v_frame_t frame = start; frame <= end; ++frame)
    {
        if (missingSet.count(frame))
            continue;

        m_Entries[frame] = std::move(Frame(basepath, name, extension, frame, padding,false));
        m_Frames.emplace_back(frame);
    }

    /* Update the media type */
    m_MediaType = MediaType::Image;
    UpdateRange();
}

MediaStruct::MediaStruct(const MediaStruct& other)
{
    /* Update the Media Type */
    m_MediaType = other.m_MediaType;

    /* Clear Existing Frames */
    m_Frames.clear();

    /* Copy the contents of the internal vector */
    m_Frames.reserve(other.m_Frames.size());

    /* Add back the frame */
    for (v_frame_t frame: other.m_Frames)
        m_Frames.emplace_back(frame);

    /* Copy the MEntries */
    m_Entries = other.m_Entries;
    m_Start = other.m_Start;
    m_End = other.m_End;
}

MediaStruct::MediaStruct(MediaStruct&& other) noexcept
    : m_MediaType(other.m_MediaType)
{
    m_MediaType = other.m_MediaType;
    m_Start = other.m_Start;
    m_End = other.m_End;

    std::swap(m_Frames, other.m_Frames);
    std::swap(m_Entries, other.m_Entries);
}

MediaStruct MediaStruct::operator=(const MediaStruct& other)
{
    /* Talking about the same entity */
    if (this == &other)
        return *this;

    /* Update the Media Type */
    m_MediaType = other.m_MediaType;

    /* Clear Existing Frames */
    m_Frames.clear();

    /* Copy the contents of the internal vector */
    m_Frames.reserve(other.m_Frames.size());

    /* Add back the frame */
    for (v_frame_t frame: other.m_Frames)
        m_Frames.emplace_back(frame);

    /* Copy the MEntries */
    m_Entries = other.m_Entries;
    m_Start = other.m_Start;
    m_End = other.m_End;

    return *this;
}

MediaStruct MediaStruct::operator=(MediaStruct&& other) noexcept
{
    if (&other == this)
        return *this;

    /* Clear Contents */
    Clear();

    m_MediaType = other.m_MediaType;
    m_Start = other.m_Start;
    m_End = other.m_End;

    /* Swap the contents of the provided Struct with ours */
    std::swap(m_Frames, other.m_Frames);
    std::swap(m_Entries, other.m_Entries);

    return *this;
}

std::string MediaStruct::Name() const
{
    return m_Entries.empty() ? "" : m_Entries.begin()->second.Name();
}

std::string MediaStruct::Extension() const
{
    return m_Entries.empty() ? "" : m_Entries.begin()->second.Extension();
}

std::string MediaStruct::Basepath() const
{
    return m_Entries.empty() ? "" : m_Entries.begin()->second.Basepath();
}

std::string MediaStruct::FirstPath() const
{
    return m_Entries.empty() ? "" : m_Entries.begin()->second.Fullpath();
}

bool MediaStruct::SingleFile() const
{
    return m_Entries.empty() ? false : m_Entries.begin()->second.SingleFile();
}

unsigned int MediaStruct::Framepadding() const
{
    return m_Entries.empty() ? 0 : m_Entries.begin()->second.Framepadding();
}

v_frame_t MediaStruct::NearestFrame(const v_frame_t frame) const
{
    /* We need the lower bound of the given frame available in the vector */
    auto it = std::lower_bound(m_Frames.begin(), m_Frames.end(), frame);

    /**
     * As the provided frame is lower than the first frame,
     * The most natural nearest frame to it is the first frame
     */
    return it != m_Frames.begin() ? *(--it) : m_Start;
}

void MediaStruct::Process()
{
    switch (m_MediaType)
    {
        case MediaType::Movie:
            ProcessMovie();
            break;

        case MediaType::Image:
            UpdateRange();
            break;
    }
}

Frame MediaStruct::First() const
{
    return m_Entries.empty() ? Frame() : m_Entries.begin()->second;
}

void MediaStruct::Add(Frame& frame)
{
    frame.Setup();
    /* Add the provided frame */
    m_Entries.insert({frame.Framenumber(), frame});
    /* Add the frame number on the vector */
    m_Frames.push_back(frame.Framenumber());
}

bool MediaStruct::Validate(const Frame& frame)
{
    /* If the underlying struct is empty this still returns false */
    if (m_Entries.empty())
        return false;

    Frame& f = m_Entries.begin()->second;

    /* Return if the provided frame is similar to the current frame */
    return f.Similar(frame);
}

MediaStruct MediaStruct::FromFile(const std::string& filepath)
{
    /**
     * Create a Media Frame for reference
     * Then start iterating over it's directory to see other files and if they are similar
     * to this path
     */
    Frame f(filepath);

    /**
     * Create a struct for updating to, representing a media sequence if needed
     */
    MediaStruct m(f, MHelper::GetMediaType(f));

    /**
     * If the type of the media is Movie or any other then we can just return the current MediaStruct from here
     * considering a movie or audio is a container on it's own and does not depend on other containers
     * Also consider if the frame does not have a frame seqeunce number like #### or %03d or %04d something similar
     */
    if (m.Type() != MediaType::Image || m.SingleFile())
        return m;

    Tools::VoidProfiler<std::chrono::duration<double>> p("Media Struct Construction");

    try
    {
        for (std::filesystem::directory_entry frame: std::filesystem::directory_iterator(f.Basepath()))
        {
            /**
             * Create a Media Entry for the next path
             */
            Frame next(frame.path().string());

            /* Ignore the frame which is the same as the original one */
            if (next == f)
                continue;

            /**
             * Check if the frame is similar to the MediaStruct representation
             * i.e. it's name and extension
             * if they are similar -> add that frame to the struct
             */
            if (m.Validate(next))
                m.Add(next);
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        VOID_LOG_ERROR(e.what());
    }

    return m;
}

void MediaStruct::Reset(Frame& frame, const MediaType& type)
{
    /* Clear underlying structs */
    if (!Empty())
        Clear();

    frame.Setup();
    /* Add the first Entry */
    m_Entries.insert({frame.Framenumber(), frame});
    /* Add the frame number on the vector */
    m_Frames.push_back(frame.Framenumber());

    /* Update the media type */
    m_MediaType = type;
}

void MediaStruct::Clear()
{
    /* Clear entries */
    m_Entries.clear();
    m_Frames.clear();

    /* Reset Media Type */
    m_MediaType = MediaType::NonMedia;
}

void MediaStruct::ProcessMovie()
{
    // /* Get the First frame since it is a Single File Movie */
    // Frame frame = m_MediaStruct.First();
    if (m_Entries.empty())
        return;

    Frame& frame = m_Entries.at(0);

    if (!frame.Valid())
        return;

    /* Media Reader */
    std::unique_ptr<VoidMPixReader> r = Forge::Instance().GetMovieReader(frame.Extension(), frame.Fullpath());

    MFrameRange frange = r->Framerange();
    VOID_LOG_INFO("MediaStruct::Movie Media Range: {0}-{1}", frange.startframe, frange.endframe);
    // m_

    // // /* Update internal framerate */
    // // m_Framerate = r->Framerate();

    m_Frames.reserve(frange.endframe - frange.startframe + 1);

    /* Add each of the Frame with the same frame and the varying frame number */
    for (v_frame_t i = frange.startframe; i < frange.endframe; i++)
    {
        frame.Setup(i);
        m_Entries[i] = frame; // Copy
        m_Frames.emplace_back(i);
        /* Update internal structures with the frame information */
        // m_Mediaframes[i] = std::move(MovieFrame(frame, i));
        // m_Entries[i] = 
        // m_Framenumbers.emplace_back(i);
    }

    m_Start = frange.startframe;
    m_End = frange.endframe - 1;

    // m_Type = Media::Type::MOVIE;
    // UpdateRange();
}

void MediaStruct::UpdateRange()
{
    if (m_Frames.empty())
        return;

    std::sort(m_Frames.begin(), m_Frames.end());

    m_Start = m_Frames.front();
    m_End = m_Frames.back();

    VOID_LOG_INFO("MediaStruct::Media Range: {0}-{1}", m_Start, m_End);
}

void MediaStruct::ClearCache()
{
    for (std::pair<const v_frame_t, Frame>& it : m_Entries)
        it.second.ClearCache();
}

/* }}} */

/* Media FS {{{ */

std::string MediaFS::ResolvedPath(const std::string& path)
{
    Frame f(path);

    if (f.Templated())
    {
        try
        {
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(f.Basepath()))
            {
                Frame searched(entry.path().string());

                /* Check if both the paths have similarity, like the path and the name and extension */
                if (searched.Similar(f))
                    return searched.Fullpath();
            }
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            VOID_LOG_INFO("Unable to resolve templated path {0}", path);
            VOID_LOG_ERROR(e.what());
        }
    }

    /* Path isn't templated for us to act on it */
    return path;
}

std::vector<MediaStruct> MediaFS::FromDirectory(const std::string& path)
{
    /* Create struct vec */
    std::vector<MediaStruct> vec;

    Tools::VoidProfiler<std::chrono::duration<double>> p("Directory Media Struct Construction");

    try
    {
        /* Start iterating over the file system frame and create an Media Entry for it */
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
        {
            Frame f(entry.path().string());

            /* Flag to control what happens with the frame */
            bool new_entry = true;

            /**
             * Iterate over what we have in our vector currently
             * i.e. the media structs to see if this frame belongs to any one of them
             * if so, this gets added there, else we create a new media struct from it
             */
            for (MediaStruct& m : vec)
            {
                /* The frame belongs to this Media Struct */
                if (m.Validate(f))
                {
                    /* Add the Media Entry to the struct */
                    m.Add(f);
                    /* Mark the new_entry flag as false to indicate this isn't new */
                    new_entry = false;
                }
            }

            /* Check if no frame in the MediaStruct adopted our newly created Media frame */
            if (new_entry)
            {
                vec.push_back(MediaStruct(f, MHelper::GetMediaType(f)));
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        VOID_LOG_ERROR(e.what());
    }

    /* Return the final vec */
    return vec;
}

std::vector<MediaStruct> MediaFS::GetAllMedia(const std::string& path, int level, int maxLevel)
{
    std::vector<MediaStruct> vec;

    Tools::VoidProfiler<std::chrono::duration<double>> p("Recursive Media Search");
    VOID_LOG_INFO("Searching Directory {0} at Level: {1}/{2}", path, level, maxLevel);

    try
    {
        for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(path))
        {
            /* Recurse through the directory if the level allows */
            if (entry.is_directory() && level <= maxLevel)
            {
                /* Get all media inside the directory */
                std::vector<MediaStruct> out = std::move(GetAllMedia(entry.path().string(), level + 1, maxLevel));

                vec.reserve(vec.size() + out.size());
                vec.insert(vec.end(), std::make_move_iterator(out.begin()), std::make_move_iterator(out.end()));
            }

            Frame f(entry.path().string());

            /* Flag to control what happens with the frame */
            bool new_entry = true;

            /**
             * Iterate over what we have in our vector currently
             * i.e. the media structs to see if this frame belongs to any one of them
             * if so, this gets added there, else we create a new media struct from it
             */
            for (MediaStruct& m : vec)
            {
                /**
                 * The frame belongs to this Media Struct don't have to add it again
                 * this search is going to be used to import media via the UndoQueue
                 * which only needs path of a single media from it
                 */
                if (m.Validate(f))
                {
                    new_entry = false;
                    break;
                }
            }

            /* Check if no frame in the MediaStruct adopted our newly created Media frame */
            if (new_entry)
            {
                vec.push_back(MediaStruct(f, MHelper::GetMediaType(f)));
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        VOID_LOG_ERROR(e.what());
    }

    return vec;
}

/* }}} */

VOID_NAMESPACE_CLOSE
