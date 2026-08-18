## This library is a composite of Player - Project - Playlist - Media - Commands related to them
# At the moment, the bridges are too dependent on one another and the components are interdependent
# This needs to be sorted
# Bridges needs to be a separate library and this library should not depend on it

# Another question is that do we need singleton bridges or a global pointer to them would make the architecture better ?
# similar to how context is used in sequencer

set(SOURCES
    # Commands
    Commands/MediaCommands.cpp
    Commands/PlaylistCommands.cpp
    Commands/TagCommands.cpp
    Commands/EffectCommands.cpp

    # Media
    Media/MediaBridge.cpp
    Media/MediaLister.cpp
    Media/MediaSearchBar.cpp
    Media/MetadataViewer.cpp
    Media/TagWidget.cpp
    Media/MediaQueue.cpp
    Media/Models/MetadataModel.cpp
    Media/Views/MediaView.cpp
    Media/Views/MetadataView.cpp
    Media/Views/QueueView.cpp
    Media/Delegates/ListDelegate.cpp
    Media/Delegates/ThumbnailDelegate.cpp

    # Player
    Player/OverlayWidget.cpp
    Player/Player.cpp
    Player/PlayerBridge.cpp
    Player/PlayerBuffer.cpp
    Player/PlayerWidget.cpp
    Player/ViewerBuffer.cpp

    # Playlist
    Playlist/PlayLister.cpp
    Playlist/Views/PlaylistView.cpp
    Playlist/Views/PlaylistMediaView.cpp
    Playlist/Delegates/ListDelegate.cpp

    # Project
    Project/Browser.cpp
    Project/Project.cpp
    Project/ProjectBridge.cpp
    Project/Importer.cpp
    Project/Views/ProjectView.cpp
    Project/Delegates/ListDelegate.cpp

    # Toolkit
    Toolkit/AnnotationController.cpp
    Toolkit/BufferSwitch.cpp
    Toolkit/ColorController.cpp
    Toolkit/ControlBar.cpp
    Toolkit/ControlScroller.cpp
    Toolkit/GridController.cpp
    Toolkit/ViewerController.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidObjects/Preferences/Preferences.h
    )
endif()

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
list(APPEND SOURCES
        ../VoidBridge/Engine.h
        ../VoidMenuSystem/MenuSystem.h
        ../VoidObjects/Core/Task.h
        ../VoidObjects/Effects/Effects.h
        ../VoidObjects/Media/MediaClip.h
        ../VoidObjects/Models/MediaModel.h
        ../VoidObjects/Models/PlaylistModel.h
        ../VoidObjects/Models/ProjectModel.h
        ../VoidObjects/Models/TagModel.h
        ../VoidObjects/Models/TagMetadataModel.h
        ../VoidObjects/Playlist/Playlist.h
        ../VoidObjects/Sequence/Sequence.h
        ../VoidObjects/Sequence/Track.h
        ../VoidObjects/Sequence/TrackItem.h
        ../VoidQExtensions/Frame.h
        ../VoidQExtensions/Label.h
        ../VoidQExtensions/ProgressTask.h
        ../VoidQExtensions/PushButton.h
        ../VoidQExtensions/Slider.h
        ../VoidQExtensions/Table.h
        ../VoidRenderer/Renderer.h
        ../VoidRenderer/VoidRenderer.h
        ../VoidTimeline/Timeline.h
        ../VoidTimeline/TimelineController.h
        ../VoidTimeline/Timeslider.h
    )
endif()

add_library(
    VoidMediaPlayer
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidMediaPlayer
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${FFMPEG_INCLUDE_DIRS}
)

target_link_libraries(
    VoidMediaPlayer
    PUBLIC
    VoidAudio
    VoidBridge
    VoidCore
    VoidExporter
    VoidIconForge
    VoidMediaBrowser
    VoidMenuSystem
    VoidObjects
    VoidQExtensions
    VoidRenderer
    VoidTimeline
    VoidUndo
)

target_link_libraries(
    VoidMediaPlayer
    PRIVATE
    Qt::Core
    Qt::Widgets
    OpenGL::GL
    OpenColorIO::OpenColorIO
    Freetype::Freetype
)

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VoidMediaPlayer PRIVATE Qt::OpenGLWidgets)
endif()

target_compile_definitions(VoidMediaPlayer PRIVATE VOID_LIB)
