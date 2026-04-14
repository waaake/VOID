// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Flipper.h"

VOID_NAMESPACE_OPEN

bool FlipOp::Evaluate(ImageRow& row)
{
    unsigned char* buffer = static_cast<unsigned char*>(row.buffer);
    for (std::size_t i = 0; i < row.width / 2; ++i)
    {
        unsigned char* left = buffer + i * row.channels;
        unsigned char* right = buffer + (row.width - 1 - i) * row.channels;

        for (std::size_t c = 0; c < row.channels; ++c)
            std::swap(left[c], right[c]);
    }

    return true;
}

VOID_NAMESPACE_CLOSE
