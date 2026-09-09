set(SOURCES

    VersionView.cpp
)

add_library(
    VoidViews
    SHARED

    ${SOURCES}
)

target_include_directories(
    VoidViews
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidViews
    PUBLIC
    VoidCore
    VoidObjects
)

target_link_libraries(
    VoidViews
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidViews PRIVATE VOID_LIB)
