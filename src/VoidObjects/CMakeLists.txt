# Build Intermediate Objects Library
set(SOURCES
    Core/Entity.cpp
    Core/Task.cpp
    Core/TaskLog.cpp
    Core/Threads.cpp

    Effects/Effects.cpp
    Effects/Bridge.cpp

    Media/MediaClip.cpp
    Media/Tag.cpp

    Models/EntityModel.cpp
    Models/ProjectModel.cpp
    Models/PlaylistModel.cpp
    Models/TagModel.cpp
    Models/TagMetadataModel.cpp

    Playlist/Playlist.cpp
    
    Project/Project.cpp

    Preferences/Preferences.cpp

    Sequence/Sequence.cpp
    Sequence/SequenceView.cpp
    Sequence/Track.cpp
    Sequence/TrackMap.cpp
    Sequence/TrackItem.cpp
    Sequence/TrackView.cpp
)

add_library(
    VoidObjects
    SHARED
    ${SOURCES}
)

target_link_libraries(
    VoidObjects
    PUBLIC
    VoidCore
    VoidRenderer
)

# Link to this library
target_link_libraries(
    VoidObjects
    PRIVATE
    Qt::Core

    # TODO: Check if MediaClip can be pushed without a thumbnail
    Qt::Gui
)

# Export symbols for windows
target_compile_definitions(VoidObjects PRIVATE VOID_LIB)
