// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "VoidApplication/Engine/Engine.h"

#ifdef _VOID_PLATFORM_WINDOWS
extern "C" 
{
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif // _VOID_PLATFORM_WINDOW

int main(int argc, char* argv[])
{
    VOID_NAMESPACE::VoidEngine engine;
    return engine.Exec(argc, argv);
}
