set(SOURCES
    IconForge.cpp
)

add_library(
    VoidIconForge
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidIconForge
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidIconForge
    PUBLIC
    VoidCore
)

target_link_libraries(
    VoidIconForge
    PRIVATE
    Qt::Core
    Qt::Widgets
)

# Export symbols
target_compile_definitions(VoidIconForge PRIVATE VOID_LIB)
