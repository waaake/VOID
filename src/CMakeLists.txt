# Package Config
include(CMakePackageConfigHelpers)
include(FreetypeInstallHelpers)

# Python Bindings
add_subdirectory(Bindings)

# Build Audio Library
add_subdirectory(VoidAudio)

# Application bridge
add_subdirectory(VoidBridge)

# Build Core Library
add_subdirectory(VoidCore)

# Dock widgets and docking ability
add_subdirectory(VoidDocker)

# Build Core Engine - UI composite Library
add_subdirectory(VoidEngine)

# Media Exporter - 
add_subdirectory(VoidExporter)

# Icon Forge
add_subdirectory(VoidIconForge)

# Media Browser
add_subdirectory(VoidMediaBrowser)

# Media core ui component (composite of project - playlist and media components)
add_subdirectory(VoidMediaPlayer)

# Menu System
add_subdirectory(VoidMenuSystem)

# Build Intermediate Objects Library
add_subdirectory(VoidObjects)

# Preferences
add_subdirectory(VoidPreferences)

# Qt Extension common widgets/functionality
add_subdirectory(VoidQExtensions)

# Build Renderer Library
add_subdirectory(VoidRenderer)

# Python scripting window-console
add_subdirectory(VoidScriptEditor)

# Sequence timeline view
add_subdirectory(VoidSequencer)

# Player Timeslider
add_subdirectory(VoidTimeline)

# Tools and other ui components serving as tools to perform certain operations
add_subdirectory(VoidToolbox)

# Undo system base
add_subdirectory(VoidUndo)

target_include_directories(
    VoidCore
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    ${OpenImageIO_INCLUDE_DIRS}
    ${RapidJSON_INCLUDE_DIRS}
)

# Logging
if (spdlog_FOUND)
    target_include_directories(
        VoidCore
        PRIVATE
        ${spdlog_INCLUDE_DIRS}
    )
endif()

# Globals
# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    include_directories(${QtOpenGLWidgets_INCLUDE_DIRS})
endif()

# Main Executable
add_executable(
    VOID
    # Core Application Engine
    VoidApplication/Engine/VoidStyle.cpp
    VoidApplication/Engine/Engine.cpp
    VoidApplication/Core/ArgumentParser.cpp
    Application.cpp
)

target_include_directories(
    VOID
    PRIVATE
    ${FREETYPE_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)

# Finally Linking to our executable
target_link_libraries(
    VOID
    PRIVATE
    VoidCore
    VoidObjects
    VoidEngine
    Qt::Widgets
    Qt::Core
    Python::Python
)

# Qt5 Resources
if (QT_MAJOR_VERSION EQUAL 5)
    qt5_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../resources/resources.qrc)
    target_sources(VOID PRIVATE ${RESOURCES})
endif()

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VOID PRIVATE Qt::OpenGLWidgets)
    # Add resources directly as qt6 converts this into a qrc file during compilation and then links with the executatble
    qt_add_resources(VOID "qresources"
        PREFIX "/resources"
        FILES
            # Icons
            ../images/VOID_Logo.svg
            ../images/VOID_Logo_900x200.png
            ../icons/icon_add.svg
            ../icons/icon_adjust.svg
            ../icons/icon_annotate.svg
            ../icons/icon_arrow_right.svg
            ../icons/icon_backspace.svg
            ../icons/icon_brush.svg
            ../icons/icon_bounce.svg
            ../icons/icon_close.svg
            ../icons/icon_close_small.svg
            ../icons/icon_cube.svg
            ../icons/icon_delete.svg
            ../icons/icon_disks.svg
            ../icons/icon_eraser.svg
            ../icons/icon_fullscreen.svg
            ../icons/icon_grid_view.svg
            ../icons/icon_hard_drive.svg
            ../icons/icon_load.svg
            ../icons/icon_list_view.svg
            ../icons/icon_maximize.svg
            ../icons/icon_minimize.svg
            ../icons/icon_missing_frame.svg
            ../icons/icon_pointer.svg
            ../icons/icon_open.svg
            ../icons/icon_play.svg
            ../icons/icon_play_2.svg
            ../icons/icon_playlist.svg
            ../icons/icon_play_selected.svg
            ../icons/icon_repeat.svg
            ../icons/icon_restore.svg
            ../icons/icon_save.svg
            ../icons/icon_save_2.svg
            ../icons/icon_settings.svg
            ../icons/icon_settings_2.svg
            ../icons/icon_sort_abc.svg
            ../icons/icon_tab_options.svg
            ../icons/icon_tag_yellow.svg
            ../icons/icon_text.svg
            ../icons/icon_views.svg
            ../icons/icon_wrench.svg
            ../icons/icon_zoom_in.svg
            ../icons/icon_zoom_out.svg

            # Fonts
            ../fonts/MaterialSymbolsSharp-Regular.ttf
            ../fonts/Roboto-Regular.ttf

            # Configs
            ../config/void-default.ocio
    )
endif()

# Install Libraries and Binaries
install(TARGETS
    VoidAudio
    VoidBridge
    VoidCore
    VoidDocker
    VoidExporter
    VoidIconForge
    VoidMediaBrowser
    VoidMediaPlayer
    VoidMenuSystem
    VoidObjects
    VoidPreferences
    VoidQExtensions
    VoidRenderer
    VoidScriptEditor
    VoidSequencer
    VoidTimeline
    VoidToolbox
    VoidEngine
    VoidUndo
    VOID
    vortex
    EXPORT VOIDTargets
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

# Install Headers
install(DIRECTORY include/ DESTINATION include)

# RPATH
# Look above in lib dir
if (APPLE) # MacOS uses @executable_path
set_target_properties(VOID PROPERTIES INSTALL_RPATH "@executable_path/../lib/" INSTALL_RPATH_USE_LINK_PATH TRUE)
elseif (UNIX) # Standard $ORIGIN RPATH
set_target_properties(VoidEngine PROPERTIES INSTALL_RPATH "$ORIGIN/../lib/")
set_target_properties(VOID PROPERTIES INSTALL_RPATH "$ORIGIN/../lib/")
endif()

set(VOID_INSTALL_CMAKE_DIR share/VOID/cmake)

# Create and install CMake Config Files
configure_package_config_file(
    ../cmake/VOIDConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/VOIDConfig.cmake"
    INSTALL_DESTINATION ${VOID_INSTALL_CMAKE_DIR}
)

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/VOIDConfig.cmake"
    DESTINATION ${VOID_INSTALL_CMAKE_DIR}
)

install(
    EXPORT VOIDTargets
    FILE VOIDTargets.cmake
    NAMESPACE VOID::
    DESTINATION ${VOID_INSTALL_CMAKE_DIR}
)

# Dll copier for windows
if (WIN32)
    find_package(zstd CONFIG REQUIRED)
    set(DEPENDENCY_TARGETS
        OpenEXR::OpenEXR
        OpenEXR::OpenEXRCore
        OpenImageIO::OpenImageIO
        OpenImageIO::OpenImageIO_Util
        OpenColorIO::OpenColorIO
        GLEW::GLEW
        zstd::libzstd_shared
        OpenEXR::IlmThread
        OpenEXR::Iex
        Imath::Imath
        Python::Python
    )

    # Install dlls for each of the dependency targets
    foreach(dep ${DEPENDENCY_TARGETS})
        install(FILES "$<TARGET_FILE:${dep}>" DESTINATION bin)
    endforeach()

    # FFmpeg dlls
    install(FILES ${FFmpeg_dlls} DESTINATION bin)

    # Freetype DLL
    install(FILES "${FREETYPE_DLL}" DESTINATION bin)

    # Turbojpeg
    install(FILES "${TurboJPEG_DLL}" DESTINATION bin)

    # windeployqt.exe /path/to/install/dir/bin
    set(QT_DEPLOY_TOOL "${QT_CMAKE_PATH}/../../../bin/windeployqt.exe")
    install(CODE "execute_process(COMMAND ${QT_DEPLOY_TOOL} ${CMAKE_INSTALL_PREFIX}/bin)")
endif()
