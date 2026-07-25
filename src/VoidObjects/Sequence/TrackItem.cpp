// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "TrackItem.h"
#include "VoidCore/Logging.h"
#include "VoidObjects/VoidContext.h"

VOID_NAMESPACE_OPEN

TrackItem::TrackItem(QObject* parent)
    : VoidObject(parent)
    , m_Media(nullptr)
    , m_Name("")
    , m_Color(90, 110, 60)
    , m_Offset(0)
    , m_Start(0)
    , m_End(0)
{
    VOID_LOG_INFO("TrackItem Created: {0}", Vuid());
}

TrackItem::TrackItem(const SharedMediaClip& media, v_frame_t start, v_frame_t end, v_frame_t offset, QObject* parent)
    : VoidObject(parent)
    , m_Media(media)
    , m_Name(media->Name())
    , m_Color(media->Color())
    , m_Offset(offset)
    , m_Start(start)
    , m_End(end)
{
    VOID_LOG_INFO("TrackItem Created: {0}", Vuid());
}

TrackItem::~TrackItem()
{
}

void TrackItem::SetMedia(const SharedMediaClip& media, v_frame_t offset)
{
    /* Update the underlying media and relevant offset */
    m_Media = media;
    m_Offset = offset;

    /* Once the Media has been updated -> emit mediaChanged signal */
    emit mediaChanged();
}

void TrackItem::SetRange(v_frame_t start, v_frame_t end)
{
    /* 
     * Update the range of the track item
     * This points to where the track item starts and end in a given Track
     */
    m_Start = start;
    m_End = end;
}

void TrackItem::Unlink()
{
    m_Media.reset();
    emit updated();
}

void TrackItem::Image(const v_frame_t frame, FloatImage& image)
{
    // Update the frame value with the offset so that we match the original media range
    v_frame_t f = frame + m_Offset;
    if (m_Media && m_Media->Contains(f))
    {
        // emit frameCached(frame);
        m_Media->Image(f, image);
    }
}

const FloatImage TrackItem::Image(v_frame_t frame)
{
    v_frame_t f = frame + m_Offset;
    if (m_Media && m_Media->Contains(f))
        return m_Media->Image(f);
    
    return nullptr;
}

void TrackItem::ClearCache(v_frame_t frame)
{
    v_frame_t f = frame + m_Offset;
    if (m_Media && m_Media->Contains(f))
        m_Media->Clear(f);
}

void TrackItem::Move(v_frame_t frame)
{
    if (m_Media)
    {
        m_Offset = m_Media->FirstFrame() - frame;
        m_Start = frame;
        m_End = m_Media->LastFrame() - m_Offset;
    }
    else
    {
        m_End = frame + Duration();
        m_Start = frame;
    }

    emit rangeChanged(m_Start, m_End);
    emit updated();
}

void TrackItem::SetColor(const QColor& color)
{
    m_Color = color;
    emit updated();
}

void TrackItem::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();

    out.AddMember("type", rapidjson::Value(TypeName(), allocator), allocator);
    // All Unlinked media gets -1 index
    out.AddMember("media_index", m_Media ? _VoidContext.ActiveProject()->MediaRow(m_Media) : -1, allocator);
    out.AddMember("name", rapidjson::Value(m_Name.c_str(), allocator), allocator);
    out.AddMember("timeline_in", static_cast<int64_t>(m_Start), allocator);
    out.AddMember("timeline_out", static_cast<int64_t>(m_End), allocator);
    out.AddMember("offset", static_cast<int64_t>(m_Offset), allocator);
    out.AddMember("r", m_Color.red(), allocator);
    out.AddMember("g", m_Color.green(), allocator);
    out.AddMember("b", m_Color.blue(), allocator);

    // TODO: Pending work with handles (head and tail) & effects added on the track item;
}

void TrackItem::Deserialize(const rapidjson::Value& in)
{
    // Media could be unlinked from the item
    int index = in["media_index"].GetInt();
    m_Media = index > -1 ? _VoidContext.ActiveProject()->MediaAt(index, 0) : nullptr;

    m_Name = in["name"].GetString();
    m_Start = in["timeline_in"].GetInt64();
    m_End = in["timeline_out"].GetInt64();
    m_Offset = in["offset"].GetInt64();
    // m_Color = std::move(QColor(in["r"].GetInt(), in["g"].GetInt(), in["b"].GetInt()));
    m_Color.setRed(in["r"].GetInt());
    m_Color.setGreen(in["g"].GetInt());
    m_Color.setBlue(in["b"].GetInt());
}

VOID_NAMESPACE_CLOSE
