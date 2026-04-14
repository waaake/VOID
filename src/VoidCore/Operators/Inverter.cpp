// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Inverter.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

bool InvertOp::Evaluate(ImageRow& row)
{
    if (row.channels < 3)
        return false;

    for (std::size_t i = 0; i < row.width; ++i)
    {
        unsigned char* pixel = row.Pixel<unsigned char>(i);

        // Skip the alpha channel inversion, it causes issues at the moment with exr based data
        // If we go back to using exrs as float data, then it might work fine, need to check that
        // exr with float are very heavy as well (4 byte per channel vs 1 byte -- 8 bits for unsigned char), 
        // need to check that as well for later
        for (std::size_t channel = 0; channel < 3; ++channel)
            pixel[channel] = 255 - pixel[channel];
    }

    return true;
}

VOID_NAMESPACE_CLOSE
