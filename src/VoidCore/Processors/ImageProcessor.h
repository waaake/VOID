// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _IMAGE_PROCESSOR_H
#define _IMAGE_PROCESSOR_H

// /* STD */
// #include <concepts>

/* Internal */
#include "Definition.h"
#include "Operator.h"
#include "VoidCore/Media/Frame.h"

VOID_NAMESPACE_OPEN

namespace ImageProcessor
{

bool Process(FloatImage& image, BaseOp* op);

bool Process(FloatImage& image, ImageRowOp* iop);
bool Process(FloatImage& image, ImageOp* iop);


// template <std::derived_from<BaseOp> _Ty>
// bool ProcessIm(FloatImage& image, _Ty* op) { return true; }

// template <>
// bool ProcessIm<ImageRowOp>(FloatImage& image, ImageRowOp* op);

// template <>
// bool ProcessIm<ImageOp>(FloatImage& image, ImageOp* op);

} // namespace ImageProcessor

VOID_NAMESPACE_CLOSE

#endif // _IMAGE_PROCESSOR_H
