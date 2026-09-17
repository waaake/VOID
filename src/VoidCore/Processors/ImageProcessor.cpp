// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* TBB */
#include <tbb/parallel_for.h>

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

bool ImageProcessor::Process(FloatImage& image, ImageOp* iop)
{
    bool status;
    Tools::VoidProfiler<std::chrono::duration<double>> p("ImageProcessor::Process");

    /**
     * Process this loop parallelly
     * Since we are dealing with each Row of Pixels for the image separately,
     * they can all be processed without having any overlap on the other
     * 
     * TODO: Check how can we safely allow one ImageOp::Evaluate to access other rows
     * maybe with or without guarantee that it will be modified
     */
    // #pragma omp parallel for
    // for (int i = 0; i < image->height; ++i)
    // {
    //     ImageRow row = image->EditableRow(i);
    //     status = iop->Evaluate(row);
    // }

    tbb::parallel_for(tbb::blocked_range<int>(0, image->height), [&](const tbb::blocked_range<int>& r) -> void
    {
        for (int i = r.begin(); i != r.end(); ++i)
        {
            ImageRow row = image->EditableRow(i);
            status = iop->Evaluate(row);
        }
    });

    return status;
}

void ImageProcessor::ProcessImage(FloatImage& image, ImageOp* iop)
{
    static ImageProcessor instance;
    instance.Process(image, iop);
}

VOID_NAMESPACE_CLOSE
