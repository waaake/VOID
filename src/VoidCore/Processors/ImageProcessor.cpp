// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* TBB */
#include <tbb/parallel_for.h>

/* Internal */
#include "ImageProcessor.h"
#include "VoidCore/Logging.h"
#include "VoidCore/Profiler.h"

VOID_NAMESPACE_OPEN

namespace ImageProcessor
{

bool Process(FloatImage& image, BaseOp* op)
{
    const OpType& t = op->OperatorType();
    switch (t)
    {
        case OpType::ImageOp: return Process(image, static_cast<ImageOp*>(op));
        case OpType::RowOp: return Process(image, static_cast<ImageRowOp*>(op));
        case OpType::NoOp:
        default: return true; // Don't have anything to process for them
    }
}

bool Process(FloatImage& image, ImageRowOp* iop)
{
    bool status;
    Tools::VoidProfiler<std::chrono::duration<double>> p("ImageProcessor::RowOp::Process");

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

bool Process(FloatImage& image, ImageOp* iop)
{
    Tools::VoidProfiler<std::chrono::duration<double>> p("ImageProcessor::ImageOp::Process");   
    return iop->Evaluate(image);
}

// template <>
// bool ProcessIm<ImageRowOp>(FloatImage& image, ImageRowOp* op)
// {
//     bool status;
//     Tools::VoidProfiler<std::chrono::duration<double>> p("ImageProcessor::RowOp::Process");

//     /**
//      * Process this loop parallelly
//      * Since we are dealing with each Row of Pixels for the image separately,
//      * they can all be processed without having any overlap on the other
//      * 
//      * TODO: Check how can we safely allow one ImageOp::Evaluate to access other rows
//      * maybe with or without guarantee that it will be modified
//      */
//     // #pragma omp parallel for
//     // for (int i = 0; i < image->height; ++i)
//     // {
//     //     ImageRow row = image->EditableRow(i);
//     //     status = iop->Evaluate(row);
//     // }

//     tbb::parallel_for(tbb::blocked_range<int>(0, image->height), [&](const tbb::blocked_range<int>& r) -> void
//     {
//         for (int i = r.begin(); i != r.end(); ++i)
//         {
//             ImageRow row = image->EditableRow(i);
//             status = op->Evaluate(row);
//         }
//     });

//     return status;    
// }

// template <>
// bool ProcessIm<ImageOp>(FloatImage& image, ImageOp* op)
// {
//     Tools::VoidProfiler<std::chrono::duration<double>> p("ImageProcessor::ImageOp::Process");    
//     return op->Evaluate(image);
// }

} // namespace ImageProcessor

VOID_NAMESPACE_CLOSE
