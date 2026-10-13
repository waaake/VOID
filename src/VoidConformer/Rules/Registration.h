// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Definition.h"
#include "FormatForge.h"

#include "Filename.h"
#include "Fullpath.h"

VOID_NAMESPACE_OPEN

namespace Internal { 

    bool RegisterInternalRules()
    {
        Forge& forge = Forge::Instance();

        forge.Register(new FilenameRule());
        forge.Register(new FullpathRule());
        return true;
    }
    
} // namespace Internal

void RegisterConformRules()
{
    Internal::RegisterInternalRules();
}

VOID_NAMESPACE_CLOSE
