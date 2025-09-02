// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "RenderTypes.h"

VOID_NAMESPACE_OPEN

namespace Renderer {

/* Annotated Vertex {{{ */

void AnnotatedVertex::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();

    out.AddMember("px", position.x, allocator);
    out.AddMember("py", position.y, allocator);

    out.AddMember("nx", normal.x, allocator);
    out.AddMember("ny", normal.y, allocator);
}

void AnnotatedVertex::Deserialize(const rapidjson::Value& in)
{
    position = { in["px"].GetFloat(), in["py"].GetFloat() };
    normal = { in["nx"].GetFloat(), in["ny"].GetFloat() };
}

/* }}} */

/* Stroke {{{ */

void Stroke::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();

    /* Vertices */
    rapidjson::Value vertices(rapidjson::kArrayType);

    for (const AnnotatedVertex& v : this->vertices)
    {
        rapidjson::Value vertex;
        v.Serialize(vertex, allocator);

        vertices.PushBack(vertex, allocator);
    }
    out.AddMember("vertices", vertices, allocator);

    /* Color */
    out.AddMember("r", color.r, allocator);
    out.AddMember("g", color.g, allocator);
    out.AddMember("b", color.b, allocator);

    /* Thickness */
    out.AddMember("thickness", thickness, allocator);
}

void Stroke::Deserialize(const rapidjson::Value& in)
{
    const rapidjson::Value::ConstArray vertices = in["vertices"].GetArray();
    this->vertices.reserve(vertices.Size());

    for (int i = 0; i < vertices.Size(); ++i)
    {
        AnnotatedVertex v;
        v.Deserialize(vertices[i]);

        this->vertices.emplace_back(v);
    }

    color = { in["r"].GetFloat(), in["g"].GetFloat(), in["b"].GetFloat() };
    thickness = in["thickness"].GetFloat();
}

/* }}} */

/* Render Text {{{ */

void RenderText::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();

    out.AddMember("text", rapidjson::Value(text.c_str(), allocator), allocator);
    
    /* Color */
    out.AddMember("r", color.r, allocator);
    out.AddMember("g", color.g, allocator);
    out.AddMember("b", color.b, allocator);

    /* Position */
    out.AddMember("x", position.x, allocator);
    out.AddMember("y", position.y, allocator);

    /* Size */
    out.AddMember("size", static_cast<unsigned int>(size), allocator);
}

void RenderText::Deserialize(const rapidjson::Value& in)
{
    text = in["text"].GetString();
    position = { in["x"].GetFloat(), in["y"].GetFloat() };
    color = { in["r"].GetFloat(), in["g"].GetFloat(), in["b"].GetFloat() };
    size = in["size"].GetUint();
}

/* }}} */

/* Annotation {{{ */

void Annotation::Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const
{
    out.SetObject();

    rapidjson::Value strokes(rapidjson::kArrayType);

    for (const Stroke& stroke : this->strokes)
    {
        rapidjson::Value s;
        stroke.Serialize(s, allocator);

        strokes.PushBack(s, allocator);
    }
    out.AddMember("strokes", strokes, allocator);

    rapidjson::Value texts(rapidjson::kArrayType);

    for (const RenderText& text : this->texts)
    {
        rapidjson::Value t;
        text.Serialize(t, allocator);

        texts.PushBack(t, allocator);
    }
    out.AddMember("texts", texts, allocator);
}

void Annotation::Deserialize(const rapidjson::Value& in)
{
    const rapidjson::Value::ConstArray strokes = in["strokes"].GetArray();
    const rapidjson::Value::ConstArray texts = in["texts"].GetArray();

    this->strokes.reserve(strokes.Size());
    this->texts.reserve(texts.Size());

    for (int i = 0; i < strokes.Size(); ++i)
    {
        Stroke s;
        s.Deserialize(strokes[i]);

        this->strokes.emplace_back(s);
    }

    for (int i = 0; i < texts.Size(); ++i)
    {
        RenderText t;
        t.Deserialize(texts[i]);

        this->texts.emplace_back(t);
    }
}

/* }}} */

} // namespace Renderer

VOID_NAMESPACE_CLOSE
