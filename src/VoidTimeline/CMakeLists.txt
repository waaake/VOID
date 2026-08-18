set(SOURCES
    Timeline.cpp
    TimelineController.cpp
    TimelineElements.cpp
    Timeslider.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidBridge/Engine.h
        ../VoidObjects/Core/Task.h
        ../VoidQExtensions/PushButton.h
    )
endif()

add_library(
    VoidTimeline
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidTimeline
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidTimeline
    PUBLIC
    VoidBridge
    VoidCore
    VoidIconForge
    VoidObjects
    VoidQExtensions
)

target_link_libraries(
    VoidTimeline
    PRIVATE
    Qt::Core
    Qt::Widgets
)

# Export symbols
target_compile_definitions(VoidTimeline PRIVATE VOID_LIB)
