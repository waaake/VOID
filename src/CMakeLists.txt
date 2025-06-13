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

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VOID PRIVATE Qt::OpenGLWidgets)
endif()
