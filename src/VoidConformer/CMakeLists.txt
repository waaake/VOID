set(SOURCES

    Rules/Filename.cpp

    Commands/ConformCommands.cpp

    Conformer.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidObjects/Effects/Effects.h
        ../VoidObjects/Core/Entity.h
        ../VoidObjects/Media/Tag.h
        ../VoidObjects/Media/MediaClip.h
        ../VoidObjects/Models/EntityModel.h
        ../VoidObjects/Models/PlaylistModel.h
        ../VoidObjects/Models/TagModel.h
        ../VoidObjects/Models/TagMetadataModel.h
        ../VoidObjects/Playlist/Playlist.h
        ../VoidObjects/Project/Project.h
        ../VoidObjects/Sequence/Sequence.h
        ../VoidObjects/Sequence/Track.h
        ../VoidObjects/Sequence/TrackItem.h
        ../VoidQExtensions/ProgressTask.h
    )
endif()

add_library(
    VoidConformer
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidConformer
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidConformer
    PUBLIC
    VoidCore
    VoidObjects
    VoidUndo
    VoidMediaPlayer
)

target_link_libraries(
    VoidConformer
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidConformer PRIVATE VOID_LIB)
