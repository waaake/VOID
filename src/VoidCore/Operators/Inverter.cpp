// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Inverter.h"
#include "VoidCore/Logging.h"

VOID_NAMESPACE_OPEN

bool InvertOp::Evaluate(const SharedPixels& image)
{
    unsigned char* buffer = static_cast<unsigned char*>(image->Writable());
    for (std::size_t i = 0; i < image->FrameSize(); ++i)
        buffer[i] = 255 - buffer[i];

    VOID_LOG_INFO("InvertOp::Process Done.");
    return true;
}

VOID_NAMESPACE_CLOSE
