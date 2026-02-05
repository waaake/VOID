// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_RENDER_TYPES_H
#define _VOID_RENDER_TYPES_H

/* STD */
#include <memory>
#include <map>
#include <string>
#include <vector>

/* GLM */
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

/* Internal */
#include "Definition.h"
#include "VoidCore/Serialization.h"

VOID_NAMESPACE_OPEN

namespace Renderer
{

enum class ChannelMode : int
{
    RED,    // Show the Red Channel
    GREEN,  // The Green channel
    BLUE,   // The Blue channel
    ALPHA,  // Alpha Channel
    RGB,    // RGB Channels leaving alpha out
    RGBA    // RGBA or all channels mostly
};

enum class ComparisonMode : int
{
    NONE,
    WIPE,
    STACK,
    HORIZONTAL,
    VERTICAL
};

enum class BlendMode : int
{
    UNDER,
    OVER,
    // MINUS,
    // DIFF,
};

static const std::map<ComparisonMode, std::string> ComparisonModesMap = 
{
    { ComparisonMode::NONE, "Off" },
    { ComparisonMode::WIPE, "Wipe" },
    { ComparisonMode::STACK, "Stack" },
    { ComparisonMode::HORIZONTAL, "Horizontal" },
    { ComparisonMode::VERTICAL, "Vertical", }
};

static const std::map<BlendMode, std::string> BlendModesMap = 
{
    { BlendMode::UNDER, "Under" },
    { BlendMode::OVER, "Over" },
};

/**
 * Holds information about the Annotation Vertex
 */
struct VOID_API AnnotatedVertex
{
    /* Position of the Vertex */
    glm::vec2 position;
    /* Normal direction of the Vertex */
    glm::vec2 normal;

    void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const;
    void Serialize(std::ostream& out) const;

    void Deserialize(const rapidjson::Value& in);
    void Deserialize(std::istream& in);

    inline const char* TypeName() const { return "Vertex"; }
};

/**
 * What is a Stroke ?
 * Stroke is a collection of Annotated Vertices which also has a 
 * defined color and brush size (thickness)
 * The brush size defines how thick will the line be drawn on the viewport
 */
struct VOID_API Stroke : public SerializableEntity
{
    std::vector<AnnotatedVertex> vertices;
    glm::vec3 color;
    float thickness;

    inline bool Empty() const noexcept { return vertices.empty(); }
    inline int Size() const noexcept { return vertices.size(); }
    inline const AnnotatedVertex* Data() const noexcept { return vertices.data(); }
    inline void Clear() { vertices.clear(); }

    virtual void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    virtual void Serialize(std::ostream& out) const override;

    virtual void Deserialize(const rapidjson::Value& in) override;
    virtual void Deserialize(std::istream& in) override;

    inline virtual const char* TypeName() const override { return "Stroke"; }
};

/**
 * RenderText is a collection of Characters to be rendered
 * as annoatated text
 */
struct VOID_API RenderText : public SerializableEntity
{
    std::string text;
    std::size_t size;
    bool active;
    bool editing;
    float length;
    glm::vec3 color;
    glm::vec2 position;

    inline bool Empty() const noexcept { return text.empty(); }
    inline bool Active() const noexcept { return active; }
    inline void Clear() { text.clear(); }

    virtual void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    virtual void Serialize(std::ostream& out) const override;

    virtual void Deserialize(const rapidjson::Value& in) override;
    virtual void Deserialize(std::istream& in) override;

    inline virtual const char* TypeName() const override { return "RenderText"; }
};

/**
 * A Structure Describing an Annotation which can be drawn on the viewport
 * The annotation comprises of 
 * Current Set of AnnotatedVertices
 * Committed/Saved Strokes (the Annotated Vertices with information like color and size)
 */
struct VOID_API Annotation : public SerializableEntity
{
    /**
     * Collection of Strokes
     */
    std::vector<Stroke> strokes;

    /**
     * Stores the Active Annotation Stroke
     */
    Stroke current;

    /**
     * Collection of Render Texts
     */
    std::vector<RenderText> texts;

    /**
     * Stores the Active text
     */
    RenderText draft;

    /**
     * Returns True if there is no active annotation and also no strokes
     */
    inline bool Empty() const
    { 
        return strokes.empty() && current.Empty() && texts.empty() && draft.Empty();
    }

    /**
     * Clears the Annotation
     */
    inline void Clear()
    {
        /* Clear any data */
        strokes.clear();
        current.Clear();
        texts.clear();
        draft.Clear();
    }

    virtual void Serialize(rapidjson::Value& out, rapidjson::Document::AllocatorType& allocator) const override;
    virtual void Serialize(std::ostream& out) const override;

    virtual void Deserialize(const rapidjson::Value& in) override;
    virtual void Deserialize(std::istream& in) override;

    inline virtual const char* TypeName() const override { return "Annotation"; }
};

typedef std::shared_ptr<Annotation> SharedAnnotation;

/**
 * A structure holding A Renderable Annotation
 */
struct AnnotationRenderData
{
    /* The Annotation to be renderer */
    Renderer::SharedAnnotation annotation;
    /* Projection for the Annotation for rendering on the viewport */
    glm::mat4 projection;
};

/**
 * Enum describing the types of draws that can happen on the Annotations Renderer
 * NONE does nothing
 * BRUSH draws annotations
 * TEXT draws text on the screen (so no brush annotation)
 * ERASER allows removing
 */
enum class DrawType
{
    /* Nothing */
    NONE,
    /* Draw/Annotate using a Brush */
    BRUSH,
    /* Draw Text */
    TEXT,
    /* Erase Strokes/Text */
    ERASER
};

} // namespace Renderer

VOID_NAMESPACE_CLOSE

#endif // _VOID_RENDER_TYPES_H
