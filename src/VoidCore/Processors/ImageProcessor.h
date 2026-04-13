// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _IMAGE_PROCESSOR_H
#define _IMAGE_PROCESSOR_H

/* Internal */
#include "Definition.h"
#include "Operator.h"
#include "VoidCore/Media/Frame.h"

VOID_NAMESPACE_OPEN

class VOID_API ImageProcessor
{
    ImageProcessor() = default;
public:
    ~ImageProcessor();
    static ImageProcessor& Instance();

    bool Process(Frame* frame, const SharedImageOp& iop);
    static void ProcessFrame(Frame* frame, const SharedImageOp& iop);
};

VOID_NAMESPACE_CLOSE

#endif // _IMAGE_PROCESSOR_H
