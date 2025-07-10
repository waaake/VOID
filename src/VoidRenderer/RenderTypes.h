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
struct AnnotatedVertex
{
    /* Position of the Vertex */
    glm::vec2 position;
    /* Normal direction of the Vertex */
    glm::vec2 normal;
};

/**
 * What is a Stroke ?
 * Stroke is a collection of Annotated Vertices which also has a 
 * defined color and brush size (thickness)
 * The brush size defines how thick will the line be drawn on the viewport
 */
struct Stroke
{
    /* All of the Annotated Vertices (points) */
    std::vector<AnnotatedVertex> vertices;

    /* Color of the Vertices (points) */
    glm::vec3 color;

    /* Size of the Brush for draw (points) */
    float thickness;

    inline bool Empty() const noexcept { return vertices.empty(); }
    inline int Size() const noexcept { return vertices.size(); }
    inline const AnnotatedVertex* Data() const noexcept { return vertices.data(); }
};

/**
 * A Structure Describing an Annotation which can be drawn on the viewport
 * The annotation comprises of 
 * Current Set of AnnotatedVertices
 * Committed/Saved Strokes (the Annotated Vertices with information like color and size)
 */
struct Annotation
{
    /**
     * Collection of Strokes
     */
    std::vector<Stroke> strokes;

    /**
     * Stores the Active Annotation
     */
    std::vector<AnnotatedVertex> annotation;

    /**
     * Stores the Active Annotation Stroke
     */
    Stroke current;

    /**
     * Returns True if there is no active annotation and also no strokes
     */
    inline bool Empty() const { return strokes.empty() && annotation.empty(); }

    /**
     * Clears the Annotation
     */
    inline void Clear()
    {
        /* Clear any data */
        strokes.clear();
        annotation.clear();
    }
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
