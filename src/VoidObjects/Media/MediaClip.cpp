// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Qt */
#include <QPixmap>

/* Internal */
#include "MediaClip.h"

VOID_NAMESPACE_OPEN

MediaClip::MediaClip(QObject* parent)
    : VoidObject(parent)
    , Media()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::MediaClip(const MediaStruct& mstruct, QObject* parent)
    : VoidObject(parent)
    , Media(mstruct)
    , m_Thumbnail()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::MediaClip(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        QObject* parent
    )
    : VoidObject(parent)
    , Media(basepath, name, extension)
    , m_Thumbnail()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::MediaClip(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding,
        QObject* parent
    )
    : VoidObject(parent)
    , Media(basepath, name, extension, start, end, padding)
    , m_Thumbnail()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}

MediaClip::MediaClip(const std::string& basepath,
        const std::string& name,
        const std::string& extension,
        v_frame_t start,
        v_frame_t end,
        unsigned int padding,
        const std::vector<v_frame_t>& missing,
        QObject* parent
    )
    : VoidObject(parent)
    , Media(basepath, name, extension, start, end, padding, missing)
    , m_Thumbnail()
{
    VOID_LOG_INFO("Clip Created: {0}", Vuid());
}


MediaClip::~MediaClip()
{
}

QPixmap MediaClip::Thumbnail()
{
    if (m_Thumbnail.isNull())
    {
        /* Grab the pointer to the image data for the first frame to be used as a thumbnail */
        SharedPixels im = Media::FirstImage();
        QImage::Format format = (im->Channels() == 3) ? QImage::Format_RGB888 : QImage::Format_RGBA8888;

        m_Thumbnail = QPixmap::fromImage(QImage(im->ThumbnailPixels(), im->Width(), im->Height(), format)).scaledToWidth(400, Qt::SmoothTransformation);
        /* Clear the data for when required */
        im->Clear();
    }

    return m_Thumbnail;
}

Renderer::SharedAnnotation MediaClip::Annotation(const v_frame_t frame) const
{
    /* We have an annotation available for the given frame */
    if (m_Annotations.find(frame) != m_Annotations.end())
        return m_Annotations.at(frame);

    /* Nothing available */
    return nullptr;
}

/* Add Annotation for a Frame */
void MediaClip::SetAnnotation(const v_frame_t frame, const Renderer::SharedAnnotation& annotation)
{
    /* Update the Annotation */
    m_Annotations[frame] = annotation;
    /* Media Clip has been updated */
    emit updated();

    VOID_LOG_INFO("Annotation Added. Frame {0}", frame);
}

void MediaClip::RemoveAnnotation(const v_frame_t frame)
{ 
    /**
     * Remove Annotation at frame
     * Since the map holds a shared pointer to the Annotation, which should get
     * deleted as there are no more references to it
     */
    m_Annotations.erase(frame);
    /* Media clip has been updated */
    emit updated();

    VOID_LOG_INFO("Annotation Removed. Frame {0}", frame);
}

void MediaClip::CacheFrame(v_frame_t frame)
{
    m_Mediaframes.at(frame).Cache();
    // emit frameCached(frame);
}

void MediaClip::UncacheFrame(v_frame_t frame)
{
    m_Mediaframes.at(frame).ClearCache();
    // emit frameUncached(frame);
}

void MediaClip::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();

    out.AddMember("type", rapidjson::Value(TypeName(), allocator), allocator);
    out.AddMember("basepath", rapidjson::Value(m_MediaStruct.Basepath().c_str(), allocator), allocator);
    out.AddMember("name", rapidjson::Value(m_MediaStruct.Name().c_str(), allocator), allocator);
    out.AddMember("extension", rapidjson::Value(m_MediaStruct.Extension().c_str(), allocator), allocator);
    out.AddMember("start", static_cast<int64_t>(m_FirstFrame), allocator);
    out.AddMember("end", static_cast<int64_t>(m_LastFrame), allocator);
    out.AddMember("singlefile", static_cast<int>(m_MediaStruct.SingleFile()), allocator);
    out.AddMember("framePadding", static_cast<unsigned int>(m_MediaStruct.Framepadding()), allocator);

    /* Save any missing frames from the media */
    rapidjson::Value missingFrames(rapidjson::kArrayType);
    for (v_frame_t i = m_FirstFrame; i < m_LastFrame; ++i)
    {
        if (m_Mediaframes.find(i) == m_Mediaframes.end())
            missingFrames.PushBack(static_cast<int64_t>(i), allocator);
    }

    out.AddMember("missingFrames", missingFrames, allocator);

    rapidjson::Value annotations(rapidjson::kArrayType);
    for (const auto& data: m_Annotations)
    {
        rapidjson::Value entry(rapidjson::kObjectType);
        entry.AddMember("frame", static_cast<int64_t>(data.first), allocator);

        /* Serialize Annotation Data */
        rapidjson::Value annotation;
        data.second->Serialize(annotation, allocator);
        entry.AddMember("data", annotation, allocator);

        annotations.PushBack(entry, allocator);
    }

    out.AddMember("annotations", annotations, allocator);
}

void MediaClip::Serialize(std::ostream& out) const
{   
    WriteString(out, m_MediaStruct.Basepath());
    WriteString(out, m_MediaStruct.Name());
    WriteString(out, m_MediaStruct.Extension());

    bool singlefile = m_MediaStruct.SingleFile();
    unsigned int padding = m_MediaStruct.Framepadding();

    out.write(reinterpret_cast<const char*>(&m_FirstFrame), sizeof(v_frame_t));
    out.write(reinterpret_cast<const char*>(&m_LastFrame), sizeof(v_frame_t));
    out.write(reinterpret_cast<const char*>(&singlefile), sizeof(singlefile));
    out.write(reinterpret_cast<const char*>(&padding), sizeof(padding));
}

void MediaClip::Deserialize(const rapidjson::Value& in)
{
    const rapidjson::Value::ConstArray missingFrames = in["missingFrames"].GetArray();

    /* A Single file is just something like a single image or maybe an audio and a movie like like .mp4, .mov etc. */
    if (in["singlefile"].GetInt())
    {
        Read(
            MediaStruct(
                in["basepath"].GetString(),
                in["name"].GetString(),
                in["extension"].GetString()
            )
        );
    }
    else if (missingFrames.Empty())
    {
        Read(
            MediaStruct(
                in["basepath"].GetString(),
                in["name"].GetString(),
                in["extension"].GetString(),
                in["start"].GetInt64(),
                in["end"].GetInt64(),
                in["framePadding"].GetUint()
            )
        );
    }
    else
    {
        std::vector<v_frame_t> missing;
        missing.reserve(missingFrames.Size());

        for (int i = 0; i < missingFrames.Size(); ++i)
            missing.emplace_back(missingFrames[i].GetInt64());

        Read(
            MediaStruct(
                in["basepath"].GetString(),
                in["name"].GetString(),
                in["extension"].GetString(),
                in["start"].GetInt64(),
                in["end"].GetInt64(),
                in["framePadding"].GetUint(),
                missing
            )
        );   
    }

    const rapidjson::Value::ConstArray annotations = in["annotations"].GetArray();
    if (!annotations.Empty())
    {
        for (int i = 0; i < annotations.Size(); ++i)
        {
            Renderer::SharedAnnotation annotation = std::make_shared<Renderer::Annotation>();
            annotation->Deserialize(annotations[i]["data"]);

            m_Annotations[annotations[i]["frame"].GetInt64()] = annotation;
        }
    }
}

void MediaClip::Deserialize(std::istream& in)
{
    std::string path = ReadString(in);
    std::string name = ReadString(in);
    std::string extension = ReadString(in);

    v_frame_t start, end;
    bool singlefile;
    unsigned int padding;

    in.read(reinterpret_cast<char*>(&start), sizeof(v_frame_t));
    in.read(reinterpret_cast<char*>(&end), sizeof(v_frame_t));
    in.read(reinterpret_cast<char*>(&singlefile), sizeof(bool));
    in.read(reinterpret_cast<char*>(&padding), sizeof(unsigned int));

    if (singlefile)
    {
        Read(MediaStruct(path, name, extension));
    }
    else
    {
        Read(MediaStruct(path, name, extension, start, end, padding));
    }
}

VOID_NAMESPACE_CLOSE
