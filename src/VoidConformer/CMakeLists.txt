set(SOURCES

    Rules/Filename.cpp

    Commands/ConformCommands.cpp

    Conformer.cpp
)

add_library(
    VoidConformer
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidConformer
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidConformer
    PUBLIC
    VoidCore
    VoidObjects
)

target_link_libraries(
    VoidConformer
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidConformer PRIVATE VOID_LIB)
