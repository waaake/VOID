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
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6OpenGLWidgets_INCLUDE_DIRS}
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

include_directories(${Qt6OpenGLWidgets_INCLUDE_DIRS})

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
    Qt6::Widgets
    Qt6::Core
    Qt6::OpenGLWidgets
    OpenGL::GL
)
