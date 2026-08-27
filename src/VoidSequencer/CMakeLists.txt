set(SOURCES
    Sequencer.cpp
    SController.cpp
    SMenu.cpp
    STimelineScene.cpp
    STimelineView.cpp
    STimelineGeometry.cpp

    Commands/SequenceCommands.cpp

    Graphics/SHandleItem.cpp
    Graphics/SPlayheadItem.cpp
    Graphics/SPreviewTrackItem.cpp
    Graphics/SRazorItem.cpp
    Graphics/STimelineEffect.cpp
    Graphics/STimelineItem.cpp
    Graphics/STrack.cpp
    Graphics/STrackItem.cpp

    Models/SSelectionModel.cpp
    Models/SHoverModel.cpp

    Widgets/STimelineRuler.cpp
    Widgets/STrackHeader.cpp
    Widgets/STrackHeaderWidget.cpp
    Widgets/SToolbar.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidMediaPlayer/Media/MediaBridge.h
        ../VoidMediaPlayer/Project/Project.h
        ../VoidObjects/Effects/Effects.h
        ../VoidObjects/Media/MediaClip.h
        ../VoidObjects/Models/EntityModel.h
        ../VoidObjects/Models/PlaylistModel.h
        ../VoidObjects/Models/ProjectModel.h
        ../VoidObjects/Models/TagModel.h
        ../VoidObjects/Models/TagMetadataModel.h
        ../VoidObjects/Playlist/Playlist.h
        ../VoidObjects/Sequence/Sequence.h
        ../VoidObjects/Sequence/Track.h
        ../VoidObjects/Sequence/TrackItem.h
        ../VoidQExtensions/Frame.h
        ../VoidQExtensions/ProgressTask.h
        ../VoidQExtensions/PushButton.h
        ../VoidTimeline/Timeline.h
        ../VoidTimeline/TimelineController.h
        ../VoidTimeline/Timeslider.h
    )
endif()

add_library(
    VoidSequencer
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidSequencer
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidSequencer
    PUBLIC
    VoidCore
    VoidMediaPlayer # Needs to be fixed when bridges are separate entities
    VoidObjects
    VoidQExtensions
    VoidTimeline
    VoidUndo
)

target_link_libraries(
    VoidSequencer
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidSequencer PRIVATE VOID_LIB)
