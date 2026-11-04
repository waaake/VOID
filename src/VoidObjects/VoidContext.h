// Copyright (c) 2025 waaake
// Licensed under the MIT License

/* Internal */
#include "Definition.h"
#include "VoidObjects/Project/Project.h"

VOID_NAMESPACE_OPEN

class VOID_API VoidContext
{
    VoidContext() = default;
public:
    static VoidContext& Instance();

    void SetActiveProject(Core::Project* project) { m_Project = project; }
    Core::Project* ActiveProject() const { return m_Project; }

private:
    Core::Project* m_Project { nullptr };
};

#define _VoidContext VoidContext::Instance()

VOID_NAMESPACE_CLOSE
