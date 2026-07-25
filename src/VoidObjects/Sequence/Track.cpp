// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cmath> // std::abs

/* Internal */
#include "Track.h"
#include "Sequence.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

PlaybackTrack::PlaybackTrack(const Sequence::TrackType& type, QObject* parent)
    : VoidObject(parent)
    , m_Recent(nullptr)
    , m_Name("")
    , m_StartFrame(0)
    , m_EndFrame(0)
    , m_Duration(0)
    , m_Visible(true)
    , m_Enabled(true)
    , m_Locked(false)
    , m_Type(type)
    // , m_Color(130, 110, 190)    /* Default Purple */
{
    VOID_LOG_INFO("Track Created: {0}", Vuid());
    connect(this, &PlaybackTrack::updated, this, [this]() -> void { m_Recent.reset(); });
}

PlaybackTrack::~PlaybackTrack()
{
}

void PlaybackTrack::SetMedia(const SharedMediaClip& media)
{
    /* Block any signals till the operation has been completed */
    bool b = blockSignals(true);
    Clear();
    blockSignals(b);

    /**
     * When the media gets added, the update and rangeChanged signals are emitted accordingly
     * hence this operation is being performed after the signals have been unblocked and we'll
     * let the other method handle the signals for this operation
     */
    AddMedia(media);
}

SharedTrackItem PlaybackTrack::AddMedia(const SharedMediaClip& media)
{
    /* Calculate the offset for reaching the first frame of media in the given timeline */
    int offset = media->FirstFrame() - m_Duration;

    /* Update the duration of the track */
    m_Duration = m_Duration + media->Duration();

    /* Update the last frame of the Track */
    m_EndFrame = m_Duration - 1;

    /* Construct the trackItem with the given media */
    /*
     * The current track is being passed as the parent indicating this track is the parent of the track item
     */
    SharedTrackItem item = std::make_shared<TrackItem>(
                                        media,
                                        media->FirstFrame() - offset,
                                        media->LastFrame() - offset,
                                        offset,
                                        this
                                    );

    // /* Set a Color on the underlying media Clip that it has been associated with this track */
    // trackItem->SetColor(m_Color);

    // /* Connect to Allow frameCache signal be invoked when media in the track item is cached */
    // connect(trackItem.get(), &TrackItem::frameCached, this, [this](int frame) { emit frameCached(frame); });

    /**
     * When the media gets added, it always gets added towards the right side which means the start frame
     * would never change in this case and only would result in a change in the last frame
     * which gets calculated based on previous last frame
     */
    m_Items.Add(item);

    /**
     * Since the media is getting added to the start frame should just remain the same,
     * while the updated end frame along with the original start should get emitted to notify others
     */
    emit rangeChanged(m_StartFrame, m_EndFrame);
    emit itemAdded(item);
    return item;
}

SharedMediaClip PlaybackTrack::Media(v_frame_t frame)
{
    if (const auto& item = GetTrackItem(frame))
        return item->GetMedia();

    return nullptr;
}

void PlaybackTrack::Clear()
{
    m_Items.Clear();
    SetRange(0, 0, false);
    emit cleared();
}

void PlaybackTrack::SetRange(int start, int end, const bool inclusive)
{
    /* Update the internal frames */
    m_StartFrame = start;
    m_EndFrame = end;

    /**
     * If inclusive is true we include the last frame in duration calculation
     *
     * so if my start frame is 1001
     * and my end frame is 1010
     * if we're not inclusive my duration is
     * 1010 - 1001 = 9 frames
     * but if we're inclusive of the last frame then the duration becomes
     * (1010 - 1001) + 1 = 10 frames
     */
    m_Duration = (end - start) + static_cast<int>(inclusive); // inclusive is bool so will be casted 0 or 1

    /* Emit the signal the the range has been modified */
    emit rangeChanged(m_StartFrame, m_EndFrame);
}

void PlaybackTrack::Cache(v_frame_t frame)
{
    if (auto item = GetTrackItem(frame))
        item->GetMedia()->Cache(frame);
}

void PlaybackTrack::Image(v_frame_t frame, FloatImage& image)
{
    if (auto item = GetTrackItem(frame))
        item->Image(frame, image);
}

const FloatImage PlaybackTrack::Image(v_frame_t frame)
{
    if (auto item = GetTrackItem(frame))
        return item->Image(frame);

    return nullptr;
}

void PlaybackTrack::ClearCache()
{
    for (SharedTrackItem& item: m_Items)
        item->GetMedia()->ClearCache();
}

void PlaybackTrack::ClearCache(v_frame_t frame)
{
    if (SharedTrackItem item = m_Items.At(frame))
        item->ClearCache(frame);
}

v_frame_t PlaybackTrack::GetSnapFrame(v_frame_t frame, const SharedTrackItem& trackitem, int threshold) const
{
    for (int i = 0; i < static_cast<int>(m_Items.Size()); ++i)
    {
        const SharedTrackItem& item = m_Items.AtIndex(i);

        if (item.get() == trackitem.get())
            continue;

        // No valid frame to snap to
        if (item->TimelineIn() > frame)
            return -1;

        if (std::abs(frame - item->TimelineOut()) < threshold)
            return item->TimelineOut() + 1;
    }

    return -1;
}

SharedTrackItem PlaybackTrack::GetTrackItem(v_frame_t frame)
{
    if (m_Recent && m_Recent->InRange(frame))
        return m_Recent;

    m_Recent = m_Items.At(frame);
    return m_Recent;
}

int PlaybackTrack::TrackIndex() const
{
    if (const auto& sequence = Sequence())
        return m_Type == Sequence::TrackType::VIDEO ? sequence->VideoTrackIndex(this) : sequence->AudioTrackIndex(this);
    return -1;
}

bool PlaybackTrack::MoveItem(const SharedTrackItem& item, v_frame_t frame)
{
    return m_Items.Move(item, frame);
}

bool PlaybackTrack::AddItem(const SharedTrackItem& item)
{
    if (m_Items.Add(item, item->TimelineIn()))
    {
        emit itemAdded(item);
        return true;
    }
    return false;
}

bool PlaybackTrack::AddItem(const SharedTrackItem& item, v_frame_t frame)
{
    if (m_Items.Add(item, frame))
    {
        item->SetTrack(this);
        // Set the timeline range based on the provided frame
        item->Move(frame);
        emit itemAdded(item);

        return true;
    }
    return false;
}

void PlaybackTrack::RemoveItem(v_frame_t frame)
{
    m_Items.Remove(frame);
    emit itemRemoved();
    emit updated();
}

void PlaybackTrack::RemoveItem(const SharedTrackItem& item)
{
    emit itemAboutToBeRemoved(item);
    m_Items.Remove(item);
    emit itemRemoved();
    emit updated();
}

void PlaybackTrack::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();

    out.AddMember("type", rapidjson::Value(TypeName(), allocator), allocator);
    out.AddMember("name", rapidjson::Value(m_Name.c_str(), allocator), allocator);
    out.AddMember("start_frame", static_cast<int64_t>(m_StartFrame), allocator);
    out.AddMember("end_frame", static_cast<int64_t>(m_EndFrame), allocator);
    out.AddMember("duration", static_cast<int64_t>(m_Duration), allocator);
    out.AddMember("visible", static_cast<int>(m_Visible), allocator);
    out.AddMember("enabled", static_cast<int>(m_Enabled), allocator);
    out.AddMember("locked", static_cast<int>(m_Locked), allocator);
    out.AddMember("track_type", static_cast<int>(m_Type), allocator);
    
    out.AddMember("item_count", static_cast<int64_t>(m_Items.Size()), allocator);

    rapidjson::Value trackitems(rapidjson::kArrayType);
    for (const SharedTrackItem& item: m_Items)
    {
        rapidjson::Value itemObject;
        item->Serialize(itemObject, allocator);

        trackitems.PushBack(itemObject, allocator);
    }

    out.AddMember("TrackItems", trackitems, allocator);
}

void PlaybackTrack::Deserialize(const rapidjson::Value& in)
{
    m_Name = in["name"].GetString();
    m_StartFrame = in["start_frame"].GetInt64();
    m_EndFrame = in["end_frame"].GetInt64();
    m_Duration = in["duration"].GetInt64();

    m_Visible = in["visible"].GetInt();
    m_Enabled = in["enabled"].GetInt();
    m_Locked = in["locked"].GetInt();
    m_Type = static_cast<Sequence::TrackType>(in["track_type"].GetInt());

    const rapidjson::Value::ConstArray trackitems = in["TrackItems"].GetArray();
    m_Items.Reserve(trackitems.Size());

    for (int i = 0; i < trackitems.Size(); ++i)
    {
        SharedTrackItem item = std::make_shared<TrackItem>(this);
        item->Deserialize(trackitems[i]);

        // m_Items.Add(item);
        // emit itemAdded(item);
        AddItem(item);
        VOID_LOG_INFO("Added item -- {0} -- TimelineIn: {1}", item->Name(), item->TimelineIn());
    }
}

VOID_NAMESPACE_CLOSE
