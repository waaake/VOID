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

void Effect::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();
    out.AddMember("name_", rapidjson::Value(m_Name.c_str(), allocator), allocator);
    out.AddMember("enabled_", static_cast<int>(m_Enabled), allocator);
    out.AddMember("timeline_in_", static_cast<int64_t>(m_TimelineIn), allocator);
    out.AddMember("timeline_out_", static_cast<int64_t>(m_TimelineOut), allocator);
    out.AddMember("color_r_", m_Color.red(), allocator);
    out.AddMember("color_g_", m_Color.green(), allocator);
    out.AddMember("color_b_", m_Color.blue(), allocator);

    for (const auto& p : m_Operator->Params())
    {
        switch (p->type)
        {
            case Param::TypeDesc::Boolean:
                out.AddMember(rapidjson::StringRef(p->name.c_str()), static_cast<int>(p->value.GetBool()), allocator);
                break;
            case Param::TypeDesc::Int:
                out.AddMember(rapidjson::StringRef(p->name.c_str()), p->value.GetInt(), allocator);
                break;
            case Param::TypeDesc::Float:
                out.AddMember(rapidjson::StringRef(p->name.c_str()), p->value.GetFloat(), allocator);
                break;
            case Param::TypeDesc::String:
                out.AddMember(rapidjson::StringRef(p->name.c_str()), rapidjson::Value(p->value.GetString().c_str(), allocator), allocator);
                break;
        }
    }
}

void Effect::Serialize(std::ostream& out) const
{
    // Internal members
    WriteString(out, m_Name);
    
    out.write(reinterpret_cast<const char*>(&m_Enabled), sizeof(m_Enabled));
    out.write(reinterpret_cast<const char*>(&m_TimelineIn), sizeof(m_TimelineIn));
    out.write(reinterpret_cast<const char*>(&m_TimelineOut), sizeof(m_TimelineOut));

    int r = m_Color.red();
    int g = m_Color.green();
    int b = m_Color.blue();

    out.write(reinterpret_cast<const char*>(&r), sizeof(r));
    out.write(reinterpret_cast<const char*>(&g), sizeof(g));
    out.write(reinterpret_cast<const char*>(&b), sizeof(b));

    // Dynamic params
    for (const auto& p : m_Operator->Params())
        out.write(reinterpret_cast<const char*>(&p->value), sizeof(p->value));
}

void Effect::Deserialize(const rapidjson::Value& in)
{
    m_Name = in["name_"].GetString();
    m_Enabled = in["enabled_"].GetInt();
    m_TimelineIn = in["timeline_in_"].GetInt64();
    m_TimelineOut = in["timeline_out_"].GetInt64();

    m_Color.setRed(in["color_r_"].GetInt());
    m_Color.setGreen(in["color_g_"].GetInt());
    m_Color.setBlue(in["color_b_"].GetInt());

    // Dynamic Params
    for (auto& p : m_Operator->Params())
    {
        switch (p->type)
        {
            case Param::TypeDesc::Boolean:
                p->SetValue(static_cast<bool>(in[p->name.c_str()].GetInt()));
                break;
            case Param::TypeDesc::Int:
                p->SetValue(in[p->name.c_str()].GetInt());
                break;
            case Param::TypeDesc::Float:
                p->SetValue(in[p->name.c_str()].GetFloat());
                break;
            case Param::TypeDesc::String:
                p->SetValue(in[p->name.c_str()].GetString());
                break;
        }
    }
}

void Effect::Deserialize(std::istream& in)
{
    m_Name = ReadString(in);

    in.read(reinterpret_cast<char*>(&m_Enabled), sizeof(m_Enabled));
    in.read(reinterpret_cast<char*>(&m_TimelineIn), sizeof(m_TimelineIn));
    in.read(reinterpret_cast<char*>(&m_TimelineOut), sizeof(m_TimelineOut));

    int r, g, b;
    in.read(reinterpret_cast<char*>(&r), sizeof(r));
    in.read(reinterpret_cast<char*>(&g), sizeof(g));
    in.read(reinterpret_cast<char*>(&b), sizeof(b));

    m_Color.setRed(r);
    m_Color.setGreen(g);
    m_Color.setBlue(b);

    // Dynamic Params
    ValueType v;
    for (auto& p : m_Operator->Params())
    {
        in.read(reinterpret_cast<char*>(&v), sizeof(v));
        p->SetValue(v);
    }
}

VOID_NAMESPACE_CLOSE
