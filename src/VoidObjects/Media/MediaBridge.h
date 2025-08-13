// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_GRAPH_H
#define _VOID_MEDIA_GRAPH_H

/* STD */
#include <vector>

/* Qt */
#include <QObject>
#include <QUndoGroup>
#include <QUndoStack>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Models/MediaModel.h"
#include "VoidObjects/Models/ProjectModel.h"
#include "VoidObjects/Project/Project.h"

VOID_NAMESPACE_OPEN

/**
 * This class acts as a singleton bridge between various components dealing with Media and MediaClips
 * The class holds all the Projects at any instant
 * Any Media which gets added, gets added to the current active project
 * this then propagates the added clip to other components by emitting mediaAdded
 * similarly when the media gets removed, a mediaAboutToBeRemoved will be emitted for all components to get
 * rid of the media before the media pointer is finally deleted internally after the media has been removed
 * from the active project
 */
class VOID_API MBridge : public QObject
{
    Q_OBJECT

protected:
    MBridge(QObject* parent = nullptr);

public:
    /* Singleton Instance */
    static MBridge& Instance();

    ~MBridge();

    /* Disable Copy */
    MBridge(const MBridge&) = delete;

    /* Disable Move */
    MBridge(MBridge&&) = delete;
    MBridge& operator=(MBridge&&) = delete;

    void AddMedia(const std::string& filepath);
    void RemoveMedia(const std::vector<QModelIndex>& media);

    /**
     * Projects
     */
    void NewProject();
    void NewProject(const std::string& name);

    void SetCurrentProject(const QModelIndex& index);
    void SetCurrentProject(int index);

    inline Project* ActiveProject() const { return m_Project; }

    /**
     * Adds Media to the Graph
     */
    bool AddMedia(const MediaStruct& mstruct);
    bool InsertMedia(const MediaStruct& mstruct, const int index);

    /**
     * Removes MediaClip
     * Emits a mediaAboutTobeRemoved signal before removing from the underlying struct
     * to allow components listening to this instance's updates to remove the entity from their structure
     */
    bool Remove(SharedMediaClip clip);
    bool Remove(const QModelIndex& index);

    inline MediaModel* DataModel() const { return m_Project->DataModel(); }
    inline ProjectModel* ProjectDataModel() const { return m_Projects; }

    /* Push an Undo Command on to the stack */
    void PushCommand(QUndoCommand* command);

    /* Returns the Menu Actions for Undo and Redo */
    QAction* CreateUndoAction(QObject* parent, const QString& prefix = QString()) const { return m_UndoGroup->createUndoAction(parent, prefix); }
    QAction* CreateRedoAction(QObject* parent, const QString& prefix = QString()) const { return m_UndoGroup->createRedoAction(parent, prefix); }

signals:
    /**
     * Media Graph Signals
     *  These will be looked up by other Void Components to see if a media has been added
     *  or is about to be removed to accordingly handle internals
     */
    void updated();
    void mediaAdded(SharedMediaClip);
    void mediaAboutToBeRemoved(SharedMediaClip);
    void projectCreated(const Project*);
    void projectChanged(const Project*);

private: /* Members */
    /* All the Available Projects */
    ProjectModel* m_Projects;
    QUndoGroup* m_UndoGroup;

    /* Current Active Project */
    Project* m_Project;

private: /* Methods */
    void SetActiveProject(Project* project);
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_GRAPH_H
