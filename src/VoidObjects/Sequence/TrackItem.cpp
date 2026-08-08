// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "TrackItem.h"
#include "VoidCore/Logging.h"
#include "VoidObjects/VoidContext.h"
#include "VoidObjects/Effects/Bridge.h"

VOID_NAMESPACE_OPEN

TrackItem::TrackItem(QObject* parent)
    : VoidObject()
    , m_Media(nullptr)
    , m_Track(reinterpret_cast<PlaybackTrack*>(parent))
    , m_Name("")
    , m_Color(90, 110, 60)
    , m_Offset(0)
    , m_TimelineIn(0)
    , m_TimelineOut(0)
    , m_SourceIn(0)
    , m_SourceOut(0)
    , m_Enabled(true)
{
    VOID_LOG_INFO("TrackItem Created: {0}", Vuid());
}

TrackItem::TrackItem(const SharedMediaClip& media, v_frame_t start, v_frame_t end, v_frame_t offset, QObject* parent)
    : VoidObject()
    , m_Media(media)
    , m_Track(reinterpret_cast<PlaybackTrack*>(parent))
    , m_Name(media->Name())
    , m_Color(media->Color())
    , m_Offset(offset)
    , m_TimelineIn(start)
    , m_TimelineOut(end)
    , m_SourceIn(media->FirstFrame())
    , m_SourceOut(media->LastFrame())
    , m_Enabled(true)
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
    m_TimelineIn = start;
    m_TimelineOut = end;
}

void TrackItem::Unlink()
{
    m_Media.reset();
    emit updated();
}

void TrackItem::SetEnabled(bool enable)
{
    m_Enabled = enable;
    emit updated();
}

Effect* TrackItem::CreateEffect(const std::string& type)
{
    if (Effect* effect = _EffectsBridge.CreateEffect(type, m_TimelineIn, m_TimelineOut))
    {
        effect->SetTimelineItem(this);
        // VOID_LOG_INFO("Effect Created -> {}", effect->Name());
        m_Effects.push_back(effect);

        emit effectCreated(effect);

        // For every effect that gets updated, the media will be set dirty
        // connect(effect, &Effect::updated, this, [this]() -> void { SetDirty(true); });
        // SetDirty(true);
        return effect;
    }

    return nullptr;
}

void TrackItem::AddEffect(Effect* effect)
{
    effect->SetTimelineItem(this);
    m_Effects.push_back(effect);
    emit effectCreated(effect);
}

void TrackItem::InsertEffect(Effect* effect, int index)
{
    effect->SetTimelineItem(this);
    m_Effects.insert(m_Effects.begin() + index, effect);
    emit effectCreated(effect);
}

bool TrackItem::RemoveEffect(const std::string& name)
{
    for (int i = static_cast<int>(m_Effects.size()) - 1; i >= 0; --i)
    {
        Effect* effect = m_Effects[i];
        if (effect->Name() == name)
        {
            emit effectAboutToBeRemoved(effect);

            effect->deleteLater();
            delete effect;
            effect = nullptr;

            m_Effects.erase(m_Effects.begin() + i);
            return true;
        }
    }

    return false;
}

void TrackItem::RemoveEffect(int index, bool destroy)
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
}

void TrackItem::ClearEffects()
{
    for (auto& effect : m_Effects)
    {
        emit effectAboutToBeRemoved(effect);
        effect->deleteLater();
        delete effect;
        effect = nullptr;
    }

    m_Effects.clear();
}

int TrackItem::EffectIndex(const Effect* const effect) const
{
    auto it = std::find(m_Effects.begin(), m_Effects.end(), effect);
    return it == m_Effects.end() ? -1 : it - m_Effects.begin();
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

void TrackItem::SetTimelineIn(v_frame_t frame)
{
    m_TimelineIn = frame;
    m_Offset = m_SourceIn - m_TimelineIn;
    m_SourceOut = frame + (m_TimelineOut - m_TimelineIn);

    emit rangeChanged(m_TimelineIn, m_TimelineOut);
}

void TrackItem::SetTimelineOut(v_frame_t frame)
{
    m_TimelineOut = frame;
    m_SourceOut = m_SourceIn + (m_TimelineOut - m_TimelineIn);

    emit rangeChanged(m_TimelineIn, m_TimelineOut);
}

void TrackItem::SetSourceIn(v_frame_t frame)
{
    int offset = m_Media->FirstFrame() - frame;
    m_SourceIn = frame;
    m_SourceOut = m_SourceIn + (m_TimelineOut - m_TimelineIn);

    m_Offset = m_SourceIn - m_TimelineIn;

    emit rangeChanged(m_TimelineIn, m_TimelineOut);
}

void TrackItem::SetSourceOut(v_frame_t frame)
{
    m_SourceOut = frame;
}

void TrackItem::TrimHead(int handle)
{
    m_SourceIn += handle;
    m_TimelineIn += handle;

    emit rangeChanged(m_TimelineIn, m_TimelineOut);
}

void TrackItem::TrimTail(int handle)
{
    m_SourceOut -= handle;
    m_TimelineOut -= handle;

    emit rangeChanged(m_TimelineIn, m_TimelineOut);
}

void TrackItem::Move(v_frame_t frame)
{
    m_Offset = m_Media ? m_SourceIn - frame : 0;
    m_TimelineOut = frame + (m_TimelineOut - m_TimelineIn);
    m_TimelineIn = frame;

    emit rangeChanged(m_TimelineIn, m_TimelineOut);
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
    out.AddMember("source_name", rapidjson::Value(m_Name.c_str(), allocator), allocator);
    out.AddMember("timeline_in", static_cast<int64_t>(m_TimelineIn), allocator);
    out.AddMember("timeline_out", static_cast<int64_t>(m_TimelineOut), allocator);
    out.AddMember("source_in", static_cast<int64_t>(m_SourceIn), allocator);
    out.AddMember("source_out", static_cast<int64_t>(m_SourceOut), allocator);
    out.AddMember("offset", static_cast<int64_t>(m_Offset), allocator);
    out.AddMember("enabled", static_cast<int>(m_Enabled), allocator);
    out.AddMember("r", m_Color.red(), allocator);
    out.AddMember("g", m_Color.green(), allocator);
    out.AddMember("b", m_Color.blue(), allocator);

    out.AddMember("effect_count", static_cast<int>(m_Effects.size()), allocator);
    rapidjson::Value effects(rapidjson::kArrayType);
    for (const auto& effect : m_Effects)
    {
        rapidjson::Value entry(rapidjson::kObjectType);
        std::string type(effect->TypeName());
        entry.AddMember("typename", rapidjson::Value(type.c_str(), allocator), allocator);

        rapidjson::Value effectObject;
        effect->Serialize(effectObject, allocator);
        entry.AddMember("effect", effectObject, allocator);

        effects.PushBack(entry, allocator);
    }

    out.AddMember("timeline_effects", effects, allocator);
}

void TrackItem::Serialize(std::ostream& out) const
{
    WriteString(out, m_Name);

    int index = m_Media ? _VoidContext.ActiveProject()->MediaRow(m_Media) : -1;
    out.write(reinterpret_cast<const char*>(&index), sizeof(index));
    out.write(reinterpret_cast<const char*>(&m_TimelineIn), sizeof(m_TimelineIn));
    out.write(reinterpret_cast<const char*>(&m_TimelineOut), sizeof(m_TimelineOut));
    out.write(reinterpret_cast<const char*>(&m_SourceIn), sizeof(m_SourceIn));
    out.write(reinterpret_cast<const char*>(&m_SourceOut), sizeof(m_SourceOut));
    out.write(reinterpret_cast<const char*>(&m_Offset), sizeof(m_Offset));
    out.write(reinterpret_cast<const char*>(&m_Enabled), sizeof(m_Enabled));

    int r = m_Color.red();
    int g = m_Color.green();
    int b = m_Color.blue();
    out.write(reinterpret_cast<const char*>(&r), sizeof(r));
    out.write(reinterpret_cast<const char*>(&g), sizeof(g));
    out.write(reinterpret_cast<const char*>(&b), sizeof(b));

    int effectsCount = static_cast<int>(m_Effects.size());
    out.write(reinterpret_cast<const char*>(&effectsCount), sizeof(effectsCount));
    for (const auto& effect : m_Effects)
    {
        WriteString(out, effect->TypeName());
        effect->Serialize(out);
    }
}

void TrackItem::Deserialize(const rapidjson::Value& in)
{
    // Media could be unlinked from the item
    int index = in["media_index"].GetInt();
    m_Media = index > -1 ? _VoidContext.ActiveProject()->MediaAt(index, 0) : nullptr;

    m_Name = in["source_name"].GetString();
    m_TimelineIn = in["timeline_in"].GetInt64();
    m_TimelineOut = in["timeline_out"].GetInt64();
    m_SourceIn = in["source_in"].GetInt64();
    m_SourceOut = in["source_out"].GetInt64();
    m_Offset = in["offset"].GetInt64();
    m_Enabled = in["enabled"].GetInt();

    m_Color.setRed(in["r"].GetInt());
    m_Color.setGreen(in["g"].GetInt());
    m_Color.setBlue(in["b"].GetInt());

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

void TrackItem::Deserialize(std::istream& in)
{
    m_Name = ReadString(in);

    int index;
    in.read(reinterpret_cast<char*>(&index), sizeof(index));
    m_Media = index > -1 ? _VoidContext.ActiveProject()->MediaAt(index, 0) : nullptr;

    in.read(reinterpret_cast<char*>(&m_TimelineIn), sizeof(m_TimelineIn));
    in.read(reinterpret_cast<char*>(&m_TimelineOut), sizeof(m_TimelineOut));
    in.read(reinterpret_cast<char*>(&m_SourceIn), sizeof(m_SourceIn));
    in.read(reinterpret_cast<char*>(&m_SourceOut), sizeof(m_SourceOut));
    in.read(reinterpret_cast<char*>(&m_Offset), sizeof(m_Offset));
    in.read(reinterpret_cast<char*>(&m_Enabled), sizeof(m_Enabled));

    int r, g, b;
    in.read(reinterpret_cast<char*>(&r), sizeof(r));
    in.read(reinterpret_cast<char*>(&g), sizeof(g));
    in.read(reinterpret_cast<char*>(&b), sizeof(b));

    m_Color.setRed(r);
    m_Color.setGreen(g);
    m_Color.setBlue(b);

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

VOID_NAMESPACE_CLOSE
