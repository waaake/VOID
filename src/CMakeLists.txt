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
)

target_include_directories(
    VoidCore
    PUBLIC
    include
    ${CMAKE_CURRENT_SOURCE_DIR}
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
            ../images/VOID_Logo.svg
            ../images/VOID_Logo_900x200.png
            ../icons/icon_adjust.svg
            ../icons/icon_annotate.svg
            ../icons/icon_arrow_right.svg
            ../icons/icon_brush.svg
            ../icons/icon_bounce.svg
            ../icons/icon_close.svg
            ../icons/icon_delete.svg
            ../icons/icon_eraser.svg
            ../icons/icon_fullscreen.svg
            ../icons/icon_maximize.svg
            ../icons/icon_minimize.svg
            ../icons/icon_missing_frame.svg
            ../icons/icon_pointer.svg
            ../icons/icon_repeat.svg
            ../icons/icon_restore.svg
            ../icons/icon_settings.svg
            ../icons/icon_text.svg
            ../icons/icon_video_player.png
            ../icons/icon_zoom_in.svg
            ../icons/icon_zoom_out.svg
    )
endif()

# Install Libraries and Binaries
install(TARGETS VoidCore VoidRenderer VoidUi LIBRARY DESTINATION lib)

# RPATH
# Look above in lib dir
if (APPLE) # MacOS uses @executable_path
set_target_properties(VOID PROPERTIES INSTALL_RPATH "@executable_path/../lib/" INSTALL_RPATH_USE_LINK_PATH TRUE)
elseif (UNIX) # Standard $ORIGIN RPATH
set_target_properties(VoidUi PROPERTIES INSTALL_RPATH "$ORIGIN/../lib/")
set_target_properties(VOID PROPERTIES INSTALL_RPATH "$ORIGIN/../lib/")
endif()

# Install dir
install(TARGETS VOID RUNTIME DESTINATION bin)
