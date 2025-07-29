# Build Ui Library
set(SOURCES
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

    # Commands
    Commands/VoidCommand.cpp
    Commands/MediaCommands.cpp

    # Media
    Media/MediaBridge.cpp
    Media/MediaClip.cpp
    Media/MediaLister.cpp
    Media/MediaSearchBar.cpp
    Media/Models/MediaModel.cpp
    Media/Views/MediaView.cpp
    Media/Delegates/ListDelegate.cpp
    Media/Delegates/ThumbnailDelegate.cpp

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

if (WIN32)
    list(APPEND SOURCES
        # Renderer   --- This is needed on windows for the moc compilation to succeed on PlayerWidget which expects
        # the staticMetaObject for BasicRenderer and VoidRenderer to be present at compile time rather link time
        ../VoidRenderer/Renderer.h
        ../VoidRenderer/VoidRenderer.h
        # TODO: Find a better way around this....
    )
endif()

add_library(
    VoidUi
    SHARED
    ${SOURCES}
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

# Mac for some reason needs this explicitly
# TODO: Check if we can statically link OCIO to VoidCore?
if (APPLE)
    target_link_libraries(VoidUi PRIVATE OpenColorIO::OpenColorIO)
endif()

# Export symbols for windows
target_compile_definitions(VoidUi PRIVATE VOID_LIB)
