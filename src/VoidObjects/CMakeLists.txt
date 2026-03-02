# Build Intermediate Objects Library
set(SOURCES
    Core/Threads.cpp
    Core/Timekeeper.cpp

    Media/MediaClip.cpp

    Models/MediaModel.cpp
    Models/ProjectModel.cpp
    Models/PlaylistModel.cpp

    Project/Project.cpp

    Playlist/Playlist.cpp

    Sequence/Sequence.cpp
    Sequence/SequenceView.cpp
    Sequence/Track.cpp
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
