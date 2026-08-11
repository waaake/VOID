// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _IMAGE_EFFECTS_H
#define _IMAGE_EFFECTS_H

/* Qt */
#include <QColor>

/* Internal */
#include "Definition.h"
#include "Operator.h"
#include "VoidObjects/VoidObject.h"

VOID_NAMESPACE_OPEN

class PlaybackTrack;
class TrackItem;

class VOID_API Effect : public VoidObject
{
    Q_OBJECT

public:
    enum class EffectType
    {
        CLIP,
        ITEM,
        TRACK
    };

public:
    Effect(ImageOp* iop, const std::string& name, QObject* parent = nullptr);
    Effect(ImageOp* iop, const std::string& name, v_frame_t in, v_frame_t out, QObject* parent = nullptr);
    ~Effect();

    TrackItem* TimelineItem() const { return m_TrackItem; }
    void SetTimelineItem(TrackItem* item);

    PlaybackTrack* Track() const { return m_Track; }
    void SetTrack(PlaybackTrack* track);

    const EffectType& GetEffectType() const { return m_Type; }

    const std::string& Name() const { return m_Name; }
    void SetName(const std::string& name);

    void SetColor(const QColor& color);
    QColor Color() const { return m_Color; }

    /**
     * @brief Set the Value on the param.
     * 
     * @param param Parameter name.
     * @param value The value to be set on the param, could be an std::string, int, float or bool.
     * @return bool true if the value was updated, else false.
     */
    bool SetValue(const std::string& param, ValueType value);

    /**
     * @brief Resets the value on the param to the default value.
     * 
     * @param param Parameter name.
     */
    void ResetValue(const std::string& param);

    /**
     * @brief Returns the Value from the parameter, if found with the provided name.
     * Else a Value bearing -1 is returned.
     * 
     * @param param Parameter name.
     * @return const ValueType& Value from the parameter.
     */
    const ValueType& Value(const std::string& param) const;

    [[nodiscard]] bool Enabled() const { return m_Enabled; }
    void SetEnabled(bool enable);

    Param* GetParam(const std::string& name) const { return m_Operator->GetParam(name); }
    ImageOp* ImageOperator() { return m_Operator; }

    const std::vector<Param*>& Params() const { return m_Operator->Params(); }

    void SetTimelineIn(v_frame_t frame);
    void SetTimelineOut(v_frame_t frame);
    void SetTimelineRange(v_frame_t start, v_frame_t end);
    v_frame_t TimelineIn() const { return m_TimelineIn; }
    v_frame_t TimelineOut() const { return m_TimelineOut; }

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const;
    void Serialize(std::ostream& out) const;
    void Deserialize(const rapidjson::Value& in);
    void Deserialize(std::istream& in);
    std::string Type() const { return m_Operator->Type(); }
    const char* TypeName() const { return "Effect"; }

signals:
    void updated();
    void valueChanged(const Param*);
    void rangeChanged(v_frame_t, v_frame_t);

private:
    ImageOp* m_Operator;
    TrackItem* m_TrackItem;
    PlaybackTrack* m_Track;
    std::string m_Name;
    QColor m_Color;
    v_frame_t m_TimelineIn, m_TimelineOut;
    EffectType m_Type;
    bool m_Enabled;
};

VOID_NAMESPACE_CLOSE

#endif // _IMAGE_EFFECTS_H
