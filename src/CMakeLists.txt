# Package Config
include(CMakePackageConfigHelpers)
include(FreetypeInstallHelpers)

# Build Core Library
add_subdirectory(VoidCore)

# Build Renderer Library
add_subdirectory(VoidRenderer)

# Build Ui Library
add_subdirectory(VoidUi)

# Depends on Qt
target_include_directories(
    VoidUi
    PRIVATE
    include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QtWidgets_INCLUDE_DIRS}
    ${QtCore_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
)

target_include_directories(
    VoidCore
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    ${OpenImageIO_INCLUDE_DIRS}
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

# Freetype
include_directories(
    ${FREETYPE_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)

# Main Executable
add_executable(
    VOID
    Application.cpp
)

# Finally Linking to our executable
target_link_libraries(
    VOID
    PRIVATE
    VoidCore
    VoidRenderer
    VoidUi
    Qt::Widgets
    Qt::Core
    OpenGL::GL
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
            ../icons/icon_adjust.svg
            ../icons/icon_annotate.svg
            ../icons/icon_arrow_right.svg
            ../icons/icon_backspace.svg
            ../icons/icon_brush.svg
            ../icons/icon_bounce.svg
            ../icons/icon_close.svg
            ../icons/icon_delete.svg
            ../icons/icon_eraser.svg
            ../icons/icon_fullscreen.svg
            ../icons/icon_grid_view.svg
            ../icons/icon_load.svg
            ../icons/icon_list_view.svg
            ../icons/icon_maximize.svg
            ../icons/icon_minimize.svg
            ../icons/icon_media_view.png
            ../icons/icon_missing_frame.svg
            ../icons/icon_pointer.svg
            ../icons/icon_play.svg
            ../icons/icon_play_selected.svg
            ../icons/icon_repeat.svg
            ../icons/icon_restore.svg
            ../icons/icon_save.svg
            ../icons/icon_settings.svg
            ../icons/icon_setting.png
            ../icons/icon_sort_abc.svg
            ../icons/icon_tab_options.svg
            ../icons/icon_text.svg
            ../icons/icon_video_player.png
            ../icons/icon_zoom_in.svg
            ../icons/icon_zoom_out.svg

            # Fonts
            ../fonts/Roboto-Regular.ttf

            # Configs
            ../config/void-default.ocio
    )
endif()

# # Global
# include_directories(PUBLIC
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
#     $<INSTALL_INTERFACE:include>
# )

# Install Libraries and Binaries
install(TARGETS VoidCore VoidRenderer VoidUi VOID
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
set_target_properties(VoidUi PROPERTIES INSTALL_RPATH "$ORIGIN/../lib/")
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

    # windeployqt.exe /path/to/install/dir/bin
    set(QT_DEPLOY_TOOL "${QT_CMAKE_PATH}/../../../bin/windeployqt.exe")
    install(CODE "execute_process(COMMAND ${QT_DEPLOY_TOOL} ${CMAKE_INSTALL_PREFIX}/bin)")
endif()
