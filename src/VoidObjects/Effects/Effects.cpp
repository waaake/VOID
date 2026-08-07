// Copyright (c) 2025 waaake
// Licensed under the MIT License

#include "Effects.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

Effect::Effect(ImageOp* iop, const std::string& name, QObject* parent)
    : Effect(iop, name, 0, 0, parent)
{
}

Effect::Effect(ImageOp* iop, const std::string& name, v_frame_t in, v_frame_t out, QObject* parent)
    : VoidObject(parent)
    , m_Operator(iop)
    , m_Name(name)
    , m_Color(225, 220, 225)
    , m_TimelineIn(in)
    , m_TimelineOut(out)
    , m_Enabled(true)
{
}

Effect::~Effect()
{
    if (m_Operator)
    {
        delete m_Operator;
        m_Operator = nullptr;
    }
}

void Effect::SetTimelineItem(TrackItem* item)
{
    m_TrackItem = item;
    emit updated();
}

void Effect::SetName(const std::string& name)
{
    m_Name = name;
    emit updated();
}

void Effect::SetColor(const QColor& color)
{
    m_Color = color;
    emit updated();
}

bool Effect::SetValue(const std::string& param, ValueType value)
{
    if (Param* p = m_Operator->GetParam(param))
    {
        p->SetValue(value);

        emit updated();
        emit valueChanged(p);
        return true;
    }

    return false;
}

void Effect::ResetValue(const std::string& param)
{
    if (Param* p = m_Operator->GetParam(param))
    {
        p->ResetValue();

        emit updated();
        emit valueChanged(p);
    }
}

const ValueType& Effect::Value(const std::string& param) const
{
    if (Param* p = m_Operator->GetParam(param))
        return p->Value();

    static ValueType invalid = std::monostate{};
    return invalid;
}

void Effect::SetEnabled(bool enable)
{
    m_Enabled = enable;
    emit updated();
}

void Effect::SetTimelineIn(v_frame_t frame)
{
    m_TimelineIn = frame;
    emit rangeChanged(m_TimelineIn, m_TimelineOut);
}

void Effect::SetTimelineOut(v_frame_t frame)
{
    m_TimelineOut = frame;
    emit rangeChanged(m_TimelineIn, m_TimelineOut);
}

void Effect::SetTimelineRange(v_frame_t start, v_frame_t end)
{
    m_TimelineIn = start;
    m_TimelineOut = end;
    emit rangeChanged(m_TimelineIn, m_TimelineOut);
}

VOID_NAMESPACE_CLOSE
