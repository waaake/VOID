// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_PROJECT_BRIDGE_H
#define _VOID_PROJECT_BRIDGE_H

/* Internal */
#include "Definition.h"
#include "VoidUi/Media/MediaBridge.h"

VOID_NAMESPACE_OPEN

/**
 * This class is a layer above the Project layer
 * Acts as a bridge dealing with the Projects and its underlying Media
 */
class ProjectBridge
{
    ProjectBridge();
public:
    static ProjectBridge& Instance();

    /* Imports Media onto the currently active project + sets the Media for playing on the Active Player */
    SharedMediaClip OpenMedia();

    /* Imports Media onto the currently active project */
    void ImportMedia();
    void ImportDirectory();

    /* Import Media ondo the specified project */
    void ImportMedia(Project* project);
    void ImportDirectory(Project* project);

    void New();
    void Open();
    void Open(const std::string& path);
    void Close();
    void Close(Project* project);

    void Save();
    void SaveAs();

    /**
     * Retrieve the Project at a given row or Index
     * The projects are stored in a 1D Model, asking for Project at a given row/index is the simplest
     * way to access it
     */
    Project* ProjectAt(int row);
    Project* ProjectAt(const QModelIndex& index);

private:
    MBridge& m_Bridge;
};

#define _ProjectBridge ProjectBridge::Instance()

VOID_NAMESPACE_CLOSE

#endif // _VOID_PROJECT_BRIDGE_H
