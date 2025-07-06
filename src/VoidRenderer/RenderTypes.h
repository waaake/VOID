#ifndef _VOID_RENDER_TYPES_H
#define _VOID_RENDER_TYPES_H

/* STD */
#include <map>

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

} // namespace Renderer

VOID_NAMESPACE_CLOSE

#endif // _VOID_RENDER_TYPES_H
