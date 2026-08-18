set(SOURCES
    Exporter.cpp
    ExportOptions.cpp
    MediaExporter.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidBridge/Engine.h
        ../VoidObjects/Media/MediaClip.h
        ../VoidObjects/Core/Task.h
        ../VoidObjects/Models/TagModel.h
        ../VoidObjects/Models/TagMetadataModel.h
        ../VoidQExtensions/Table.h
    )
endif()

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
