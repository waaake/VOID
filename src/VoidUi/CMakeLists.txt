# Build Ui Library
add_library(
    VoidUi
    SHARED
    Docker.cpp
    Events.h
    MediaLister.cpp
    MediaItem.cpp
    PlayerWidget.cpp
    PlayerWindow.cpp
    Timeslider.cpp
    VoidStyle.cpp
)

# Link to this library
target_link_libraries(
    VoidUi
    VoidCore
    VoidRenderer
    Qt5::Widgets
    Qt5::Core
    OpenGL::GL
)
