// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <iomanip>
#include <regex>
#include <sstream>
#include <unordered_set>

/* Internal */
#include "MediaFilesystem.h"
#include "Logging.h"
#include "Profiler.h"

VOID_NAMESPACE_OPEN

/* MHelper {{{ */

MediaType MHelper::GetMediaType(const MEntry& entry)
{
    /* If it's a Movie */
    if (IsMovie(entry))
        return MediaType::Movie;
    else if (IsImage(entry))
        return MediaType::Image;

    /**
     * For Now Audio isn't interpreted
     * TODO: Add Audio format detection
     */

    return MediaType::NonMedia;
}

/* }}} */

/* MEntry {{{ */

MEntry::MEntry()
    : m_Path("")
    , m_Basepath("")
    , m_Name("")
    , m_Extension("")
    , m_Framenumber(0)
    , m_FramePadding(0)
    , m_SingleFile(false)
{
}

MEntry::MEntry(const std::string& path)
    : MEntry()
{
    /* Parse the provided path and update internal token */
    Parse(path);
}

MEntry::MEntry(const std::string& basepath, const std::string& name, const std::string& extension, v_frame_t frame, unsigned int padding, bool singlefile)
    : m_Basepath(basepath)
    , m_Name(name)
    , m_Extension(extension)
    , m_Framenumber(frame)
    , m_FramePadding(padding)
    , m_SingleFile(singlefile)
{
    std::filesystem::path path = std::filesystem::path(m_Basepath) / name;
    if (!singlefile)
        path += "." + PaddedFrame(frame);

    path += "." + extension;
    m_Path = path.string();
}

MEntry::~MEntry()
{
}

MEntry::MEntry(MEntry&& other) noexcept
    : m_Path(other.m_Path)
    , m_Basepath(other.m_Basepath)
    , m_Name(other.m_Name)
    , m_Extension(other.m_Extension)
    , m_FramePadding(other.m_FramePadding)
    , m_Framenumber(other.m_Framenumber)
    , m_SingleFile(other.m_SingleFile)
    , m_Templated(other.m_Templated)
{
}

MEntry& MEntry::operator=(MEntry&& other) noexcept
{
    if (&other == this)
        return *this;
    
    m_Path = other.m_Path;
    m_Basepath = other.m_Basepath;
    m_Name = other.m_Name;
    m_Extension = other.m_Extension;
    m_FramePadding = other.m_FramePadding;
    m_Framenumber = other.m_Framenumber;
    m_SingleFile = other.m_SingleFile;
    m_Templated = other.m_Templated;

    return *this;
}

MEntry::MEntry(const MEntry& other)
    : m_Path(other.m_Path)
    , m_Basepath(other.m_Basepath)
    , m_Name(other.m_Name)
    , m_Extension(other.m_Extension)
    , m_FramePadding(other.m_FramePadding)
    , m_Framenumber(other.m_Framenumber)
    , m_SingleFile(other.m_SingleFile)
    , m_Templated(other.m_Templated)
{
}

MEntry& MEntry::operator=(const MEntry& other)
{
    if (&other == this)
        return *this;

    m_Path = other.m_Path;
    m_Basepath = other.m_Basepath;
    m_Name = other.m_Name;
    m_Extension = other.m_Extension;
    m_FramePadding = other.m_FramePadding;
    m_Framenumber = other.m_Framenumber;
    m_SingleFile = other.m_SingleFile;
    m_Templated = other.m_Templated;

    return *this;
}

void MEntry::Parse(const std::string& path)
{
    /* Update the fullpath */
    m_Path = path;

    std::filesystem::path filepath(path);

    /* The base path for the file */
    m_Basepath = filepath.parent_path().string();
    /* Grab the filename from the rest of the base path */
    std::string filename = filepath.filename().string();

    /* From the last . try to get the extension and other parts */
    size_t lastDot = filename.find_last_of(".");
    std::string remaining = filename.substr(0, lastDot);

    m_Extension = filename.substr(lastDot + 1);

    lastDot = remaining.find_last_of(".");
    std::string framestring = remaining.substr(lastDot + 1);

    /* Validate a case where file is named 1.ext or 1001.ext */
    if (framestring == remaining)
    {
        m_Name = remaining;
        /* It can be considered as a single file */
        m_SingleFile = true;

        /* No need to proceed further */
        return;
    }

    /**
     * Image is a sequence or atleast follows a sequential naming convention
     * Check if the framestring is a valid number before trying to cast that to one
     */
    if (ValidFrame(framestring))
    {
        m_FramePadding = framestring.size();
        m_Framenumber = std::stol(framestring);
        m_Name = remaining.substr(0, lastDot);
    }
    else if (ValidTemplate(framestring))
    {
        m_Templated = true;
        m_Name = remaining.substr(0, lastDot);
    }
    else /* In case we don't have an image sequence, just a standard image */
    {
        m_Name = remaining;
        /* Update the state so we know it is a single file */
        m_SingleFile = true;
    }
}

bool MEntry::ValidFrame(const std::string& framestring) const
{
    if (framestring.empty())
        return false;

    for (char c: framestring)
    {
        if (!std::isdigit(c))
            return false;
    }

    /* Valid frame */
    return true;
}

bool MEntry::ValidTemplate(const std::string& framestring) const
{
    std::regex hashPattern("(#+)");
    std::regex printfPattern("%0\\d+d");

    return std::regex_search(framestring, hashPattern) || std::regex_search(framestring, printfPattern);
}

std::string MEntry::PaddedFrame(v_frame_t frame) const
{
    std::stringstream ss;
    ss << std::setw(m_FramePadding) << std::setfill('0') << frame;
    return ss.str();
}

/* }}} */

/* Media Struct {{{ */

MediaStruct::MediaStruct()
    : m_MediaType(MediaType::NonMedia)
{
}

MediaStruct::MediaStruct(const MEntry& entry, const MediaType& type)
{
    /* Setup Media */
    Reset(entry, type);
}

MediaStruct::~MediaStruct()
{
}

MediaStruct::MediaStruct(const std::string& basepath, const std::string& name, const std::string& extension)
{
    /* This is a Single file */
    MEntry e = MEntry(basepath, name, extension, 0, 0, true);
    Reset(e, MHelper::GetMediaType(e));
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
        m_Entries[frame] = std::move(MEntry(basepath, name, extension, frame, padding,false));
        m_Frames.emplace_back(frame);
    }

    /* Update the media type */
    m_MediaType = MediaType::Image;
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

        m_Entries[frame] = std::move(MEntry(basepath, name, extension, frame, padding,false));
        m_Frames.emplace_back(frame);
    }

    /* Update the media type */
    m_MediaType = MediaType::Image;
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
}

MediaStruct::MediaStruct(MediaStruct&& other) noexcept
    : m_MediaType(other.m_MediaType)
{
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

    return *this;
}

MediaStruct MediaStruct::operator=(MediaStruct&& other) noexcept
{
    if (&other == this)
        return *this;

    /* Clear Contents */
    Clear();

    m_MediaType = other.m_MediaType;

    /* Swap the contents of the provided Struct with ours */
    std::swap(m_Frames, other.m_Frames);
    std::swap(m_Entries, other.m_Entries);

    return *this;
}

std::string MediaStruct::Name() const
{
    /* Empty container */
    if (m_Entries.empty())
        return "";

    return m_Entries.begin()->second.Name();
}

std::string MediaStruct::Extension() const
{
    /* Empty container */
    if (m_Entries.empty())
        return "";

    return m_Entries.begin()->second.Extension();
}

std::string MediaStruct::Basepath() const
{
    /* Empty container */
    if (m_Entries.empty())
        return "";

    return m_Entries.begin()->second.Basepath();
}

std::string MediaStruct::FirstPath() const
{
    /* Empty container */
    if (m_Entries.empty())
        return "";

    return m_Entries.begin()->second.Fullpath();
}

bool MediaStruct::SingleFile() const
{
    /* Empty container */
    if (m_Entries.empty())
        return false;

    return m_Entries.begin()->second.SingleFile();
}

unsigned int MediaStruct::Framepadding() const
{
    /* Empty container */
    if (m_Entries.empty())
        return 0;

    return m_Entries.begin()->second.Framepadding();
}

MEntry MediaStruct::First() const
{
    /* Empty container */
    if (m_Entries.empty())
        return MEntry();

    return m_Entries.begin()->second;
}

void MediaStruct::Add(const MEntry& entry)
{
    /* Add the provided entry */
    m_Entries.insert({entry.Framenumber(), entry});
    /* Add the frame number on the vector */
    m_Frames.push_back(entry.Framenumber());
}

bool MediaStruct::Validate(const MEntry& entry)
{
    /* If the underlying struct is empty this still returns false */
    if (m_Entries.empty())
        return false;

    MEntry& e = m_Entries.begin()->second;

    /* Return if the provided entry is similar to the current entry */
    return e.Similar(entry);
}

MediaStruct MediaStruct::FromFile(const std::string& filepath)
{
    /**
     * Create a Media Entry for reference
     * Then start iterating over it's directory to see other files and if they are similar
     * to this path
     */
    MEntry e(filepath);

    /**
     * Create a struct for updating to, representing a media sequence if needed
     */
    MediaStruct m(e, MHelper::GetMediaType(e));

    /**
     * If the type of the media is Movie or any other then we can just return the current MediaStruct from here
     * considering a movie or audio is a container on it's own and does not depend on other containers
     * Also consider if the entry does not have a frame seqeunce number like #### or %03d or %04d something similar
     */
    if (m.Type() != MediaType::Image || m.SingleFile())
        return m;

    Tools::VoidProfiler<std::chrono::duration<double>> p("Media Struct Construction");

    try
    {
        for (std::filesystem::directory_entry entry: std::filesystem::directory_iterator(e.Basepath()))
        {
            /**
             * Create a Media Entry for the next path
             */
            MEntry next(entry.path().string());

            /* Ignore the entry which is the same as the original one */
            if (next == e)
                continue;

            /**
             * Check if the entry is similar to the MediaStruct representation
             * i.e. it's name and extension
             * if they are similar -> add that entry to the struct
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

void MediaStruct::Reset(const MEntry& entry, const MediaType& type)
{
    /* Clear underlying structs */
    if (!Empty())
        Clear();

    /* Add the first Entry */
    m_Entries.insert({entry.Framenumber(), entry});
    /* Add the frame number on the vector */
    m_Frames.push_back(entry.Framenumber());

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

/* }}} */

/* Media FS {{{ */

std::string MediaFS::ResolvedPath(const std::string& path)
{
    MEntry e(path);

    if (e.Templated())
    {
        try
        {
            for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(e.Basepath()))
            {
                MEntry searched(entry.path().string());

                /* Check if both the paths have similarity, like the path and the name and extension */
                if (searched.Similar(e))
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
        /* Start iterating over the file system entry and create an Media Entry for it */
        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(path))
        {
            MEntry e(entry.path().string());

            /* Flag to control what happens with the entry */
            bool new_entry = true;

            /**
             * Iterate over what we have in our vector currently
             * i.e. the media structs to see if this entry belongs to any one of them
             * if so, this gets added there, else we create a new media struct from it
             */
            for (MediaStruct& m : vec)
            {
                /* The entry belongs to this Media Struct */
                if (m.Validate(e))
                {
                    /* Add the Media Entry to the struct */
                    m.Add(e);
                    /* Mark the new_entry flag as false to indicate this isn't new */
                    new_entry = false;
                }
            }

            /* Check if no entry in the MediaStruct adopted our newly created Media entry */
            if (new_entry)
            {
                vec.push_back(MediaStruct(e, MHelper::GetMediaType(e)));
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
        for (std::filesystem::directory_entry entry : std::filesystem::directory_iterator(path))
        {
            /* Recurse through the directory if the level allows */
            if (entry.is_directory() && level <= maxLevel)
            {
                /* Get all media inside the directory */
                std::vector<MediaStruct> out = std::move(GetAllMedia(entry.path().string(), level + 1, maxLevel));

                vec.reserve(vec.size() + out.size());
                vec.insert(vec.end(), std::make_move_iterator(out.begin()), std::make_move_iterator(out.end()));
            }

            MEntry e(entry.path().string());

            /* Flag to control what happens with the entry */
            bool new_entry = true;

            /**
             * Iterate over what we have in our vector currently
             * i.e. the media structs to see if this entry belongs to any one of them
             * if so, this gets added there, else we create a new media struct from it
             */
            for (MediaStruct& m : vec)
            {
                /**
                 * The entry belongs to this Media Struct don't have to add it again
                 * this search is going to be used to import media via the UndoQueue
                 * which only needs path of a single media from it
                 */
                if (m.Validate(e))
                {
                    new_entry = false;
                    break;
                }
            }

            /* Check if no entry in the MediaStruct adopted our newly created Media entry */
            if (new_entry)
            {
                vec.push_back(MediaStruct(e, MHelper::GetMediaType(e)));
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
