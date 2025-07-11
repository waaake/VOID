# Build Ui Library
add_library(
    VoidUi
    SHARED
    About.cpp
    Browser.cpp
    Docker.cpp
    Events.h
    BaseWindow.cpp
    MediaBridge.cpp
    MediaClip.cpp
    MediaLister.cpp
    MediaItem.cpp
    PlayerWidget.cpp
    PlayerWindow.cpp
    Sequence.cpp
    TitleBar.cpp
    Track.cpp
    TrackItem.cpp
    ViewerBuffer.cpp
    VoidStyle.cpp
    Preferences/Preferences.cpp
    Preferences/PreferencesUI.cpp
    Preferences/PlayerPreferences.cpp
    Timeline/Timeline.cpp
    Timeline/TimelineElements.cpp
    Timeline/Timeslider.cpp
    Toolkit/AnnotationController.cpp
    Toolkit/BufferSwitch.cpp
    Toolkit/ControlBar.cpp
    Toolkit/ControlScroller.cpp
    QExtensions/Frame.cpp
    QExtensions/Label.cpp
    QExtensions/PushButton.cpp
    QExtensions/Slider.cpp
    QExtensions/SpinBox.cpp
    QExtensions/ToolTip.cpp
)

target_link_libraries(
    VoidUi
    PUBLIC
    VoidCore
    VoidRenderer
)

# Link to this library
target_link_libraries(
    VoidUi
    PRIVATE
    Qt::Widgets
    Qt::Core
    OpenGL::GL
)

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VoidUi PRIVATE Qt::OpenGLWidgets)
endif()

# Export symbols for windows
target_compile_definitions(VoidUi PRIVATE VOID_LIB)
