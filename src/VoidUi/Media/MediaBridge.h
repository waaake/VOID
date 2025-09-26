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
#include "VoidUi/Project/Project.h"

VOID_NAMESPACE_OPEN

/* Forward Decl to make this a friend */
class ProjectBridge;

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
    void ImportDirectory(const std::string& directory, bool progressive = true) { m_Project->ImportDirectory(directory, progressive); }

    /**
     * Projects
     */
    void NewProject();
    void NewProject(const std::string& name);

    void SetCurrentProject(const QModelIndex& index);
    void SetCurrentProject(int index);
    void SetCurrentProject(Project* project);

    inline Project* ActiveProject() const { return m_Project; }

    /**
     * Adds Media to the Graph
     */
    bool AddMedia(const MediaStruct& mstruct);    
    bool InsertMedia(const MediaStruct& mstruct, const int index);

    /**
     * Playlist
     */
    Playlist* NewPlaylist();
    Playlist* NewPlaylist(const std::string& name);
    void SetCurrentPlaylist(const QModelIndex& index);
    void SetCurrentPlaylist(int row);

    inline Playlist* ActivePlaylist() const { return m_Project->ActivePlaylist(); }
    inline Playlist* PlaylistAt(const QModelIndex& index) const { return m_Project->PlaylistAt(index); }
    inline Playlist* PlaylistAt(int row, int column) const { return m_Project->PlaylistAt(row, column); }

    /**
     * Removes MediaClip
     * Emits a mediaAboutTobeRemoved signal before removing from the underlying struct
     * to allow components listening to this instance's updates to remove the entity from their structure
     */
    bool Remove(SharedMediaClip clip);
    bool Remove(const QModelIndex& index);

    inline MediaModel* DataModel() const { return m_Project->DataModel(); }
    inline ProjectModel* ProjectDataModel() const { return m_Projects; }
    inline SharedMediaClip MediaAt(const QModelIndex& index) const { return m_Project->MediaAt(index); }
    inline SharedMediaClip MediaAt(int row, int column) const { return m_Project->MediaAt(row, column); }
    inline SharedMediaClip PlaylistMediaAt(const QModelIndex& index) const { return m_Project->PlaylistMediaAt(index); }
    inline SharedMediaClip PlaylistMediaAt(int row, int column) const { return m_Project->PlaylistMediaAt(row, column); }

    /* Push an Undo Command on to the stack */
    void PushCommand(QUndoCommand* command);

    /* Returns the Menu Actions for Undo and Redo */
    QAction* CreateUndoAction(QObject* parent, const QString& prefix = QString()) const { return m_UndoGroup->createUndoAction(parent, prefix); }
    QAction* CreateRedoAction(QObject* parent, const QString& prefix = QString()) const { return m_UndoGroup->createRedoAction(parent, prefix); }

    /**
     * Project I/O processors
     */
    bool Save();
    bool Save(const std::string& path, const std::string& name, const EtherFormat::Type& type = EtherFormat::Type::ASCII);
    void Load(const std::string& path);
    bool CloseProject(bool force = false);
    bool CloseProject(Project* project, bool force = false);

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
    void playlistCreated(const Playlist*);
    void playlistChanged(const Playlist*);

private: /* Members */
    /* All the Available Projects */
    ProjectModel* m_Projects;
    QUndoGroup* m_UndoGroup;

    /* Current Active Project */
    Project* m_Project;

private: /* Methods */
    void SetActiveProject(Project* project);
    friend class ProjectBridge;
};

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_GRAPH_H
