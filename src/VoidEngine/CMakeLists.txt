# Build Ui Library
set(SOURCES
    # Base Window
    BaseWindow/About.cpp
    BaseWindow/BaseWindow.cpp
    BaseWindow/PlayerWindow.cpp
    BaseWindow/TitleBar.cpp
    BaseWindow/WorkspaceManager.cpp
    BaseWindow/StartupWindow.cpp

    # Core Engine Globals
    Engine/Bridge.cpp
    Engine/Globals.cpp
    Engine/Controller.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidBridge/Engine.h
        ../VoidDocker/Docker.h
        ../VoidDocker/DockManager.h
        ../VoidMediaPlayer/Media/MediaBridge.h
        ../VoidMediaPlayer/Media/MediaLister.h
        ../VoidMediaPlayer/Player/Player.h
        ../VoidMediaPlayer/Player/PlayerBuffer.h
        ../VoidMediaPlayer/Player/PlayerWidget.h
        ../VoidMediaPlayer/Player/ViewerBuffer.h
        ../VoidMediaPlayer/Playlist/PlayLister.h
        ../VoidMediaPlayer/Project/Project.h
        ../VoidMenuSystem/MenuSystem.h
        ../VoidObjects/Models/EntityModel.h
        ../VoidObjects/Models/PlaylistModel.h
        ../VoidObjects/Models/ProjectModel.h
        ../VoidObjects/Models/TagMetadataModel.h
        ../VoidObjects/Playlist/Playlist.h
        ../VoidObjects/Sequence/Sequence.h
        ../VoidObjects/Sequence/Track.h
        ../VoidObjects/Sequence/TrackItem.h
        ../VoidObjects/Core/Task.h
        ../VoidObjects/Effects/Effects.h
        ../VoidObjects/Media/MediaClip.h
        ../VoidObjects/Models/TagModel.h
        ../VoidObjects/Preferences/Preferences.h
        ../VoidPreferences/PreferencesUI.h
        ../VoidQExtensions/Frame.h
        ../VoidQExtensions/Label.h
        ../VoidQExtensions/PushButton.h
        ../VoidQExtensions/LineEdit.h
        ../VoidQExtensions/ProgressTask.h
        ../VoidQExtensions/Slider.h
        ../VoidRenderer/Renderer.h
        ../VoidRenderer/VoidRenderer.h
        ../VoidSequencer/Sequencer.h
        ../VoidToolbox/Editor/Properties.h
        ../VoidTimeline/Timeline.h
        ../VoidTimeline/TimelineController.h
        ../VoidTimeline/Timeslider.h
    )
endif()

add_library(
    VoidEngine
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidEngine
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${FREETYPE_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)

target_link_libraries(
    VoidEngine
    PUBLIC
    VoidAudio
    VoidBridge
    VoidCore
    VoidDocker
    VoidExporter
    VoidIconForge
    VoidMediaBrowser
    VoidMediaPlayer
    VoidMenuSystem
    VoidObjects
    VoidPreferences
    VoidQExtensions
    VoidRenderer
    VoidScriptEditor
    VoidSequencer
    VoidTimeline
    VoidToolbox
    VoidUndo
)

# Link to this library
target_link_libraries(
    VoidEngine
    PRIVATE
    Qt::Widgets
    Qt::Core
    OpenGL::GL
    # pybind11::embed
    Python::Python
    OpenColorIO::OpenColorIO
)

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VoidEngine PRIVATE Qt::OpenGLWidgets)
endif()

# Mac for some reason needs this explicitly
# TODO: Check if we can statically link OCIO to VoidCore?
if (APPLE)
    target_link_libraries(VoidEngine PRIVATE OpenColorIO::OpenColorIO)
endif()

# Export symbols for windows
target_compile_definitions(VoidEngine PRIVATE VOID_LIB)
