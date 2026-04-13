// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ImageProcessor.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

ImageProcessor& ImageProcessor::Instance()
{
    static ImageProcessor instance;
    return instance;
}

ImageProcessor::~ImageProcessor()
{
}

bool ImageProcessor::Process(Frame* frame, const SharedImageOp& iop)
{
    bool status = false;
    if (frame->Dirty())
    {
        status = iop->Evaluate(frame->Image());
        frame->SetDirty(false);

        VOID_LOG_INFO("Processed frame");
    }

    return status;
}

void ImageProcessor::ProcessFrame(Frame* frame, const SharedImageOp& iop)
{
    static ImageProcessor instance;
    instance.Process(frame, iop);
}

VOID_NAMESPACE_CLOSE
