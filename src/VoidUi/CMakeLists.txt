# Build Ui Library
add_library(
    VoidUi
    SHARED
    About.cpp
    Browser.cpp
    ControlBar.cpp
    ControlScroller.cpp
    Docker.cpp
    Events.h
    FramelessWindow.cpp
    MediaClip.cpp
    MediaLister.cpp
    MediaItem.cpp
    PlayerWidget.cpp
    PlayerWindow.cpp
    Sequence.cpp
    Timeline.cpp
    TitleBar.cpp
    Track.cpp
    TrackItem.cpp
    ViewerBuffer.cpp
    VoidStyle.cpp
)

# Link to this library
target_link_libraries(
    VoidUi
    PRIVATE
    VoidCore
    VoidRenderer
    Qt6::Widgets
    Qt6::Core
    Qt6::OpenGLWidgets
    OpenGL::GL
)
