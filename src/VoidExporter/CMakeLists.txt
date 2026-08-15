set(SOURCES
    Exporter.cpp
    ExportOptions.cpp
    MediaExporter.cpp
)

add_library(
    VoidExporter
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidExporter
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidExporter
    PUBLIC
    VoidBridge
    VoidCore
    VoidMediaBrowser
    VoidObjects
    VoidToolbox
)

target_link_libraries(
    VoidExporter
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidExporter PRIVATE VOID_LIB)
