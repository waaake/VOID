// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <cmath> // std::abs

/* Internal */
#include "Track.h"
#include "Sequence.h"
#include "VoidCore/Logging.h"
#include "VoidObjects/Effects/Bridge.h"

VOID_NAMESPACE_OPEN

PlaybackTrack::PlaybackTrack(const Sequence::TrackType& type, QObject* parent)
    : VoidObject(parent)
    , m_Recent(nullptr)
    , m_Name("")
    , m_StartFrame(0)
    , m_EndFrame(0)
    , m_Duration(0)
    , m_MaxEffects(0)
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

int PlaybackTrack::EffectIndex(const Effect* const effect) const
{
    auto it = std::find(m_Effects.begin(), m_Effects.end(), effect);
    return it == m_Effects.end() ? -1 : it - m_Effects.begin();
}

Effect* PlaybackTrack::CreateEffect(const std::string& effect)
{
    // Can only be created at the timeline level, if there are no current track items on it
    if (m_Items.Empty())
    {
        PlaybackSequence* sequence = Sequence();
        if (Effect* created = _EffectsBridge.CreateEffect(effect, sequence->StartFrame(), sequence->EndFrame()))
        {
            created->SetTrack(this);
            m_Effects.push_back(created);
            emit effectAdded(created);
            emit maxEffectsChanged();
            return created;
        }
    }
    return nullptr;
}

Effect* PlaybackTrack::CreateEffect(const std::string& effect, const std::string& name)
{
    // Can only be created at the timeline level, if there are no current track items on it
    if (m_Items.Empty())
    {
        PlaybackSequence* sequence = Sequence();
        if (Effect* created = _EffectsBridge.CreateEffect(effect, name, sequence->StartFrame(), sequence->EndFrame()))
        {
            created->SetTrack(this);
            m_Effects.push_back(created);
            emit effectAdded(created);
            emit maxEffectsChanged();
            return created;
        }
    }
    return nullptr;
}

void PlaybackTrack::InsertEffect(Effect* effect, int index)
{
    effect->SetTrack(this);
    m_Effects.insert(m_Effects.begin() + index, effect);
    emit effectAdded(effect);
    emit maxEffectsChanged();
}

void PlaybackTrack::RemoveEffect(int index, bool destroy)
{
    Effect*& effect = m_Effects[index];
    emit effectAboutToBeRemoved(effect);
    if (destroy)
    {
        effect->deleteLater();
        delete effect;
        effect = nullptr;
    }

    m_Effects.erase(m_Effects.begin() + index);
    emit effectRemoved();
    emit maxEffectsChanged();
}

void PlaybackTrack::ClearEffects()
{
    for (int i = static_cast<int>(m_Effects.size()) - 1; i >= 0; --i)
        RemoveEffect(i, true);
}

Effect* PlaybackTrack::CreateEffect(const SharedTrackItem& item, const std::string& effect)
{
    Effect* created = item->CreateEffect(effect);
    if (created)
    {
        emit effectAdded(created);
        CalculateMaxEffects(item);
    }
    return created;
}

Effect* PlaybackTrack::CreateEffect(const SharedTrackItem& item, const std::string& effect, const std::string& name)
{
    Effect* created = item->CreateEffect(effect, name);
    if (created)
    {
        emit effectAdded(created);
        CalculateMaxEffects(item);
    }
    return created;
}

void PlaybackTrack::CopyEffect(Effect* effect, const SharedTrackItem& item)
{
    Effect* copied = _EffectsBridge.Copy(effect);
    copied->SetTimelineItem(item.get());
    copied->SetTimelineRange(item->TimelineIn(), item->TimelineOut());

    item->AddEffect(copied);

    // Recalc now that a new effect was added
    CalculateMaxEffects();
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
    ClearEffects();
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
    if (m_Recent && m_Recent->InTimelineRange(frame))
        return m_Recent;

    m_Recent = m_Items.At(frame);
    return m_Recent;
}

int PlaybackTrack::Index() const
{
    if (const auto& sequence = Sequence())
        return m_Type == Sequence::TrackType::VIDEO ? sequence->VideoTrackIndex(this) : sequence->AudioTrackIndex(this);
    return -1;
}

bool PlaybackTrack::RazorAt(v_frame_t frame)
{
    if (SharedTrackItem item = m_Items.At(frame))
    {
        // Can't cut on the start and end frames
        if (frame == item->TimelineIn() || frame == item->TimelineOut())
            return false;

        v_frame_t out = item->TimelineOut();
        item->SetTimelineOut(frame);

        SharedMediaClip media = item->GetMedia();
        int offset = item->SourceOut() - frame + 1;

        SharedTrackItem nitem = std::make_shared<TrackItem>(media, frame + 1, out, offset, this);
        nitem->SetSourceIn(item->SourceOut() + 1);

        // The requested and the frame where the other item starts
        m_Razored.insert(frame);
        m_Razored.insert(frame + 1);

        // Copy all effects as well
        for (const auto& effect : item->Effects())
            CopyEffect(effect, nitem);

        m_Items.Add(nitem);
        emit itemAdded(nitem);

        return true;
    }
    return false;
}

bool PlaybackTrack::MergeCut(v_frame_t frame)
{
    if (IsRazored(frame) && IsRazored(frame + 1))
    {
        SharedTrackItem first = m_Items.At(frame);
        SharedTrackItem second = m_Items.At(frame + 1);

        // Media does not match and hence we can't merge
        if (first->GetMedia().get() != second->GetMedia().get())
            return false;

        first->SetTimelineOut(second->TimelineOut());
        RemoveItem(second);

        m_Razored.erase(frame);
        m_Razored.erase(frame + 1);
        return true;
    }

    return false;
}

bool PlaybackTrack::MoveItem(const SharedTrackItem& item, v_frame_t frame)
{
    return m_Items.Move(item, frame);
}

bool PlaybackTrack::OffsetItem(const SharedTrackItem& item, int offset)
{
    return m_Items.Offset(item, offset);
}

bool PlaybackTrack::AddItem(const SharedTrackItem& item)
{
    if (m_Items.Add(item, item->TimelineIn()))
    {
        CalculateMaxEffects(item);
        item->SetTrack(this);
        emit itemAdded(item);
        return true;
    }
    return false;
}

bool PlaybackTrack::AddItem(const SharedTrackItem& item, v_frame_t frame)
{
    // Effects Track -- Can't allow Track items to be moved to this unless effects are cleared
    // Need to double check this behaviour in other dccs too
    if (m_Effects.size()) return false;

    if (m_Items.Add(item, frame))
    {
        CalculateMaxEffects(item);
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
    int effects = item->NumEffects();
    emit itemAboutToBeRemoved(item);
    m_Items.Remove(item);

    if (effects == m_MaxEffects)
        CalculateMaxEffects();

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

    out.AddMember("effect_count", static_cast<int>(m_Effects.size()), allocator);
    rapidjson::Value effects(rapidjson::kArrayType);
    for (const auto& effect : m_Effects)
    {
        rapidjson::Value entry(rapidjson::kObjectType);
        std::string type(effect->Type());
        entry.AddMember("typename", rapidjson::Value(type.c_str(), allocator), allocator);

        rapidjson::Value effectObject;
        effect->Serialize(effectObject, allocator);
        entry.AddMember("effect", effectObject, allocator);

        effects.PushBack(entry, allocator);
    }

    out.AddMember("timeline_effects", effects, allocator);
}

void PlaybackTrack::Serialize(std::ostream& out) const
{
    WriteString(out, m_Name);
    out.write(reinterpret_cast<const char*>(&m_StartFrame), sizeof(m_StartFrame));
    out.write(reinterpret_cast<const char*>(&m_EndFrame), sizeof(m_EndFrame));
    out.write(reinterpret_cast<const char*>(&m_Duration), sizeof(m_Duration));
    out.write(reinterpret_cast<const char*>(&m_Visible), sizeof(m_Visible));
    out.write(reinterpret_cast<const char*>(&m_Enabled), sizeof(m_Enabled));
    out.write(reinterpret_cast<const char*>(&m_Locked), sizeof(m_Locked));
    out.write(reinterpret_cast<const char*>(&m_Type), sizeof(m_Type));

    int64_t itemCount = static_cast<int64_t>(m_Items.Size());
    out.write(reinterpret_cast<const char*>(&itemCount), sizeof(itemCount));

    for (int i = 0; i < itemCount; ++i)
        m_Items.AtIndex(i)->Serialize(out);
    
    int effectsCount = static_cast<int>(m_Effects.size());
    out.write(reinterpret_cast<const char*>(&effectsCount), sizeof(effectsCount));

    for (const auto& effect : m_Effects)
    {
        WriteString(out, effect->Type());
        effect->Serialize(out);
    }
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
        AddItem(item);
    }

    const rapidjson::Value::ConstArray effects = in["timeline_effects"].GetArray();
    m_Effects.reserve(effects.Size());
    for (int i = 0; i < effects.Size(); ++i)
    {
        std::string type = effects[i]["typename"].GetString();
        if (Effect* effect = _EffectsBridge.CreateEffect(type))
        {
            effect->Deserialize(effects[i]["effect"]);
            m_Effects.push_back(effect);
        }
    }
}

void PlaybackTrack::Deserialize(std::istream& in)
{
    m_Name = ReadString(in);
    in.read(reinterpret_cast<char*>(&m_StartFrame), sizeof(m_StartFrame));
    in.read(reinterpret_cast<char*>(&m_EndFrame), sizeof(m_EndFrame));
    in.read(reinterpret_cast<char*>(&m_Duration), sizeof(m_Duration));
    in.read(reinterpret_cast<char*>(&m_Visible), sizeof(m_Visible));
    in.read(reinterpret_cast<char*>(&m_Enabled), sizeof(m_Enabled));
    in.read(reinterpret_cast<char*>(&m_Locked), sizeof(m_Locked));
    in.read(reinterpret_cast<char*>(&m_Type), sizeof(m_Type));

    int64_t itemCount;
    in.read(reinterpret_cast<char*>(&itemCount), sizeof(itemCount));

    for (int i = 0; i < itemCount; ++i)
    {
        SharedTrackItem item = std::make_shared<TrackItem>(this);
        item->Deserialize(in);
        AddItem(item);
    }

    int effectsCount;
    in.read(reinterpret_cast<char*>(&effectsCount), sizeof(effectsCount));
    m_Effects.reserve(effectsCount);

    for (int i = 0; i < effectsCount; ++i)
    {
        std::string type = ReadString(in);
        if (Effect* effect = _EffectsBridge.CreateEffect(type))
        {
            effect->Deserialize(in);
            m_Effects.push_back(effect);
        }
    }
}

void PlaybackTrack::CalculateMaxEffects(const SharedTrackItem& item)
{
    int previous = m_MaxEffects;
    m_MaxEffects = std::max(m_MaxEffects, item->NumEffects());

    if (previous != m_MaxEffects)
        emit maxEffectsChanged();
}

void PlaybackTrack::CalculateMaxEffects()
{
    m_MaxEffects = 0;
    for (const auto& item : m_Items)
        m_MaxEffects = std::max(m_MaxEffects, item->NumEffects());

    emit maxEffectsChanged();
}

VOID_NAMESPACE_CLOSE
