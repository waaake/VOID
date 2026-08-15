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

if (WIN32)
    list(APPEND SOURCES
        # Renderer   --- This is needed on windows for the moc compilation to succeed on PlayerWidget which expects
        # the staticMetaObject for BasicRenderer and VoidRenderer to be present at compile time rather link time
        ../VoidRenderer/Renderer.h
        ../VoidRenderer/VoidRenderer.h

        # # Objects   --- This is needed on windows for the moc compilation to succeed on PlayerWidget which expects
        # # the staticMetaObject for MediaClip, Track, TrackItem, Sequence and MediaBridge to be present at compile time rather link time
        ../VoidObjects/Core/Task.h

        ../VoidObjects/Effects/Effects.h

        ../VoidObjects/Media/MediaClip.h

        ../VoidObjects/Playlist/Playlist.h

        ../VoidObjects/Sequence/Sequence.h
        ../VoidObjects/Sequence/Track.h
        ../VoidObjects/Sequence/TrackItem.h

        ../VoidObjects/Models/MediaModel.h
        ../VoidObjects/Models/PlaylistModel.h
        ../VoidObjects/Models/ProjectModel.h
        ../VoidObjects/Models/TagModel.h
        ../VoidObjects/Models/TagMetadataModel.h
        # TODO: Find a better way around this....
    )
endif()

add_library(
    VoidEngine
    SHARED
    ${SOURCES}
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
