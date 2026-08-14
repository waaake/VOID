set(SOURCES
    Models/MediaFilesModel.cpp
    Views/FileView.cpp

    Browser.cpp
    FileDialog.cpp
)

add_library(
    VoidMediaBrowser
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidMediaBrowser
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidMediaBrowser
    PUBLIC
    VoidCore
    VoidObjects
    VoidQExtensions
)

target_link_libraries(
    VoidMediaBrowser
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidMediaBrowser PRIVATE VOID_LIB)
