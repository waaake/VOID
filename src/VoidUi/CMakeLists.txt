# Build Ui Library
add_library(
    VoidUi
    SHARED
    About.cpp
    Browser.cpp
    BufferSwitch.cpp
    ControlBar.cpp
    ControlScroller.cpp
    Docker.cpp
    Events.h
    BaseWindow.cpp
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
    QExtensions/PushButton.cpp
)

# Link to this library
target_link_libraries(
    VoidUi
    PRIVATE
    VoidCore
    VoidRenderer
    Qt::Widgets
    Qt::Core
    OpenGL::GL
)

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VoidUi PRIVATE Qt::OpenGLWidgets)
endif()
