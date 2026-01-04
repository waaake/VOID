// Copyright (c) 2025 waaake
// Licensed under the MIT License

#ifndef _VOID_MEDIA_GRAPH_H
#define _VOID_MEDIA_GRAPH_H

/* STD */
#include <vector>

/* Qt */
#include <QByteArray>
#include <QMenu>
#include <QObject>
#include <QUndoGroup>
#include <QUndoStack>

/* Internal */
#include "Definition.h"
#include "VoidObjects/Media/MediaClip.h"
#include "VoidObjects/Sequence/Track.h"
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
    void RemoveMedia(const QModelIndex& index);
    void RemoveMedia(const std::vector<QModelIndex>& indexes);

    void AddToPlaylist(const QModelIndex& index);
    void AddToPlaylist(const std::vector<QModelIndex>& indexes);
    void AddToPlaylist(const QModelIndex& index, Playlist* playlist);
    void AddToPlaylist(const std::vector<QModelIndex>& indexes, Playlist* playlist);
    void AddToPlaylist(QByteArray& data);
    void AddToPlaylist(QByteArray& data, Playlist* playlist);

    void RemoveFromPlaylist(const QModelIndex& index);
    void RemoveFromPlaylist(const std::vector<QModelIndex>& indexes);
    void RemoveFromPlaylist(const QModelIndex& index, Playlist* playlist);
    void RemoveFromPlaylist(const std::vector<QModelIndex>& indexes, Playlist* playlist);
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
    bool AddMedia(MediaStruct&& mstruct);
    bool AddMedia(const MediaStruct& mstruct);    
    bool InsertMedia(MediaStruct&& mstruct, int index);
    bool InsertMedia(const MediaStruct& mstruct, int index);

    /**
     * Playlist
     */
    Playlist* NewPlaylist();
    Playlist* NewPlaylist(const std::string& name);
    void RemovePlaylist(const QModelIndex& index);
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
    inline SharedMediaClip LastMedia() const { return m_Project->LastMedia(); }
    inline SharedMediaClip PlaylistMediaAt(const QModelIndex& index) const { return m_Project->PlaylistMediaAt(index); }
    inline SharedMediaClip PlaylistMediaAt(int row, int column) const { return m_Project->PlaylistMediaAt(row, column); }

    /**
     * Packs the media indexes from the project/playlist into a byteArray which can be sent
     * over to other components this data is packed for use in drag-drop operations and any other places required
     */
    QByteArray PackIndexes(const std::vector<QModelIndex>& indexes) const;
    /* Unpacks incoming ByteArray stream to fetch media based on the indexes from the current project */
    std::vector<SharedMediaClip> UnpackProjectMedia(QByteArray& data) const;
    /* Unpacks incoming ByteArray stream to fetch media based on the indexes from the current active playlist */
    std::vector<SharedMediaClip> UnpackPlaylistMedia(QByteArray& data) const;
    /* Creates a track with the provided set of media */
    SharedPlaybackTrack AsTrack(const std::vector<SharedMediaClip>& media) const;

    /* Push an Undo Command on to the stack */
    void PushCommand(QUndoCommand* command);

    /* Returns the Menu Actions for Undo and Redo */
    QAction* CreateUndoAction(QObject* parent, const QString& prefix = QString()) const { return m_UndoGroup->createUndoAction(parent, prefix); }
    QAction* CreateRedoAction(QObject* parent, const QString& prefix = QString()) const { return m_UndoGroup->createRedoAction(parent, prefix); }
    QMenu* RecentProjectsMenu(QMenu* parent = nullptr);

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

    /* Menu Holding the recent projects */
    QMenu* m_RecentProjectsMenu;

private: /* Methods */
    void DefaultProject();
    void SetActiveProject(Project* project);
    void ResetProjectsMenu();
    friend class ProjectBridge;
};

#define _MediaBridge MBridge::Instance()

VOID_NAMESPACE_CLOSE

#endif // _VOID_MEDIA_GRAPH_H
