// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Inverter.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

bool InvertOp::Evaluate(ImageRow& row)
{
    for (std::size_t i = 0; i < row.width; ++i)
    {
        unsigned char* pixel = row.Pixel<unsigned char>(i);
        for (std::size_t channel = 0; channel < row.channels; ++channel)
            pixel[channel] = 255 - pixel[channel];
    }

    return true;
}

VOID_NAMESPACE_CLOSE
