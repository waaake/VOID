set(SOURCES
    VoidCommand.cpp
)

add_library(
    VoidUndo
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidUndo
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidUndo
    PRIVATE
    Qt::Core
    Qt::Widgets
)

# Export
target_compile_definitions(VoidUndo PRIVATE VOID_LIB)
