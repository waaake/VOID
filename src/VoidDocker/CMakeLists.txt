set(SOURCES
    Docker.cpp
    DockManager.cpp
    DockPanel.cpp
    DockSplitter.cpp
)

add_library(
    VoidDocker
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidDocker
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidDocker
    PUBLIC
    VoidCore
    VoidIconForge
    VoidQExtensions
)

target_link_libraries(
    VoidDocker
    PRIVATE
    Qt::Core
    Qt::Widgets
)

# Export
target_compile_definitions(VoidDocker PRIVATE VOID_LIB)
