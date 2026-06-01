// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "ArgumentParser.h"

VOID_NAMESPACE_OPEN

namespace ArgParser {

EngineArguments ParseArgs(int argc, char** argv)
{
    EngineArguments args;

    for (int i = 0; i < argc; ++i)
    {
        std::string arg = argv[i];

        if ("--project" == arg && i + 1 < argc)
            args.project = argv[++i];
        else if ("--media" == arg && i + 1 < argc)
            args.media = argv[++i];
        else if ("--basic" == arg)
            args.basic = true;
        else if ("--scale" == arg)
            args.scale = std::atof(argv[++i]);
        else if ("--framerate" == arg)
            args.framerate = std::atof(argv[++i]);
        else if ("--audio" == arg)
            args.audio = true;
        else if ("--help" == arg)
            args.help = true;
    }

    return args;
}

} // namespace ArgParser

VOID_NAMESPACE_CLOSE
