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

void AnnotatedVertex::Serialize(std::ostream& out) const
{
    out.write(reinterpret_cast<const char*>(&position.x), sizeof(position.x));
    out.write(reinterpret_cast<const char*>(&position.y), sizeof(position.y));

    out.write(reinterpret_cast<const char*>(&normal.x), sizeof(normal.x));
    out.write(reinterpret_cast<const char*>(&normal.y), sizeof(normal.y));
}

void AnnotatedVertex::Deserialize(const rapidjson::Value& in)
{
    position = { in["px"].GetFloat(), in["py"].GetFloat() };
    normal = { in["nx"].GetFloat(), in["ny"].GetFloat() };
}

void AnnotatedVertex::Deserialize(std::istream& in)
{
    // in.read(reinterpret_cast<char*>(&position), sizeof(position));
    // in.read(reinterpret_cast<char*>(&normal), sizeof(normal));
    float x, y;
    in.read(reinterpret_cast<char*>(&x), sizeof(x));
    in.read(reinterpret_cast<char*>(&y), sizeof(y));
    position = { x, y };

    in.read(reinterpret_cast<char*>(&x), sizeof(x));
    in.read(reinterpret_cast<char*>(&y), sizeof(y));
    normal = { x, y };
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

void Stroke::Serialize(std::ostream& out) const
{
    uint32_t count = vertices.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const AnnotatedVertex& v : vertices)
        v.Serialize(out);

    out.write(reinterpret_cast<const char*>(&color.r), sizeof(color.r));
    out.write(reinterpret_cast<const char*>(&color.g), sizeof(color.g));
    out.write(reinterpret_cast<const char*>(&color.b), sizeof(color.b));

    out.write(reinterpret_cast<const char*>(&thickness), sizeof(thickness));
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

void Stroke::Deserialize(std::istream& in)
{
    uint32_t count = 0;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    vertices.reserve(count);

    for (uint32_t i = 0; i < count; ++i)
    {
        AnnotatedVertex v;
        v.Deserialize(in);

        vertices.emplace_back(v);
    }

    /* Color */
    float r, g, b;
    in.read(reinterpret_cast<char*>(&r), sizeof(r));
    in.read(reinterpret_cast<char*>(&g), sizeof(g));
    in.read(reinterpret_cast<char*>(&b), sizeof(b));
    color = { r, g, b };

    in.read(reinterpret_cast<char*>(&thickness), sizeof(thickness));
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

void RenderText::Serialize(std::ostream& out) const
{
    WriteString(out, text);

    out.write(reinterpret_cast<const char*>(&color.r), sizeof(color.r));
    out.write(reinterpret_cast<const char*>(&color.g), sizeof(color.g));
    out.write(reinterpret_cast<const char*>(&color.b), sizeof(color.b));

    out.write(reinterpret_cast<const char*>(&position.x), sizeof(position.x));
    out.write(reinterpret_cast<const char*>(&position.y), sizeof(position.y));

    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
}

void RenderText::Deserialize(const rapidjson::Value& in)
{
    text = in["text"].GetString();
    position = { in["x"].GetFloat(), in["y"].GetFloat() };
    color = { in["r"].GetFloat(), in["g"].GetFloat(), in["b"].GetFloat() };
    size = in["size"].GetUint();
}

void RenderText::Deserialize(std::istream& in)
{
    text = std::move(ReadString(in));

    float a, b, c;
    in.read(reinterpret_cast<char*>(&a), sizeof(a));
    in.read(reinterpret_cast<char*>(&b), sizeof(b));
    in.read(reinterpret_cast<char*>(&c), sizeof(c));
    color = { a, b, c };

    in.read(reinterpret_cast<char*>(&a), sizeof(a));
    in.read(reinterpret_cast<char*>(&b), sizeof(b));
    position = { a, b };

    in.read(reinterpret_cast<char*>(&size), sizeof(size));
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

void Annotation::Serialize(std::ostream& out) const
{
    uint64_t strokeCount = strokes.size();
    out.write(reinterpret_cast<const char*>(&strokeCount), sizeof(strokeCount));

    for (const Stroke& stroke : strokes)
        stroke.Serialize(out);
    
    uint64_t textCount = texts.size();
    out.write(reinterpret_cast<const char*>(&textCount), sizeof(textCount));

    for (const RenderText& text : texts)
        text.Serialize(out);
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

void Annotation::Deserialize(std::istream& in)
{
    uint64_t strokeCount = 0;
    in.read(reinterpret_cast<char*>(&strokeCount), sizeof(strokeCount));
    strokes.reserve(strokeCount);

    for (uint64_t i = 0; i < strokeCount; ++i)
    {
        Stroke s;
        s.Deserialize(in);

        strokes.emplace_back(s);
    }

    uint64_t textCount = 0;
    in.read(reinterpret_cast<char*>(&textCount), sizeof(textCount));
    texts.reserve(textCount);
    
    for (uint64_t i = 0; i < textCount; ++i)
    {
        RenderText text;
        text.Deserialize(in);

        texts.emplace_back(text);
    }
}

/* }}} */

} // namespace Renderer

VOID_NAMESPACE_CLOSE
