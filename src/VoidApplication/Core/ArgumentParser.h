// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* STD */
#include <string>

/* Internal */
#include "Definition.h"

VOID_NAMESPACE_OPEN

namespace ArgParser {

struct EngineArguments
{
    /* File paths to the specified argument */
    std::string project;
    std::string media;

    float framerate = 0.f;
    bool basic = false;
};

EngineArguments ParseArgs(int argc, char** argv);

} // namespace ArgParser

VOID_NAMESPACE_CLOSE
