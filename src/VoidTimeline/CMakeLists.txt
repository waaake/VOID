set(SOURCES
    Timeline.cpp
    TimelineController.cpp
    TimelineElements.cpp
    Timeslider.cpp
)

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
