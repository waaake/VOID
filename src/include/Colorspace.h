#ifndef _VOID_COLORSPACE_H
#define _VOID_COLORSPACE_H

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

enum class ColorSpace
{
    /* Linear */
    Linear,

    /* Display Rec.709 */
    Rec709,

    /* Gamma encoding ~2.2 */
    sRGB,

    /* ACES Color Grading Space */
    ACEScg,

    /* Arri LogC */
    LogC,

    /* Custom, LUT Baked */
    Custom,
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_COLORSPACE_H
