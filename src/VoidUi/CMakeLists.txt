# Build Ui Library
add_library(
    VoidUi
    SHARED
    About.cpp
    Browser.cpp
    Events.h
    PlayerWidget.cpp
    Sequence.cpp
    Track.cpp
    TrackItem.cpp
    ViewerBuffer.cpp
    VoidStyle.cpp
    
    # Base Window
    BaseWindow/Docker.cpp
    BaseWindow/BaseWindow.cpp
    BaseWindow/PlayerWindow.cpp
    BaseWindow/TitleBar.cpp

    # Media
    MediaBridge.cpp
    MediaClip.cpp
    MediaLister.cpp
    MediaItem.cpp
    MediaSearchBar.cpp

    # Preferences
    Preferences/Preferences.cpp
    Preferences/PreferencesUI.cpp
    Preferences/PlayerPreferences.cpp

    # Timeline
    Timeline/Timeline.cpp
    Timeline/TimelineElements.cpp
    Timeline/Timeslider.cpp

    # Toolkit
    Toolkit/AnnotationController.cpp
    Toolkit/BufferSwitch.cpp
    Toolkit/ControlBar.cpp
    Toolkit/ControlScroller.cpp

    # QExtensions
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
