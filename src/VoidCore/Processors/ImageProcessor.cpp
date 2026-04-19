// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ImageProcessor.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

ImageProcessor& ImageProcessor::Instance()
{
    static ImageProcessor instance;
    return instance;
}

// ImageProcessor::~ImageProcessor()
// {
// }

bool ImageProcessor::Process(Frame* frame, const SharedImageOp& iop)
{
    bool status = false;
    if (frame->Dirty())
    {
        Tools::VoidProfiler<std::chrono::duration<double>> p("ImageProcessor::Process");

        /**
         * Process this loop parallelly
         * Since we are dealing with each Row of Pixels for the image separately,
         * they can all be processed without having any overlap on the other
         * 
         * TODO: Check how can we safely allow one ImageOp::Evaluate to access other rows
         * maybe with or without guarantee that it will be modified
         */
        SharedPixels image = frame->Image(false);

        #pragma omp parallel for
        for (int i = 0; i < image->Height(); ++i)
        {
            ImageRow row = image->Row(i);
            iop->Evaluate(row);
        }

        frame->SetDirty(false);
        status = true;
    }

    return status;
}

void ImageProcessor::ProcessFrame(Frame* frame, const SharedImageOp& iop)
{
    static ImageProcessor instance;
    instance.Process(frame, iop);
}

VOID_NAMESPACE_CLOSE
