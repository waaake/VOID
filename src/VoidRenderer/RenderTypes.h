#ifndef _VOID_RENDER_TYPES_H
#define _VOID_RENDER_TYPES_H

/* STD */
#include <vector>
#include <memory>
#include <map>

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

struct Stroke
{
    /* All of the Annotated Vertices (points) */
    std::vector<AnnotatedVertex> vertices;

    /* Color of the Vertices (points) */
    glm::vec3 color;

    /* Thickness of the Vertices (points) */
    float thickness;
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
struct RenderableAnnotation
{
    /* The Committed Vertices */
    std::vector<Stroke> strokes;

    /* Ongoing Annotation */
    AnnotatedVertex current;

    /* The Projection Matrix for the Render */
    glm::mat4 projection;
};

typedef std::shared_ptr<RenderableAnnotation> SharedRenderableAnnotation;

} // namespace Renderer

VOID_NAMESPACE_CLOSE

#endif // _VOID_RENDER_TYPES_H
