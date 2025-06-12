# Build Render Library
add_library(
    VoidRenderer
    SHARED
    Renderer.cpp
    RendererStatus.cpp
)

target_include_directories(
    VoidRenderer
    PRIVATE
    include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Qt6Widgets_INCLUDE_DIRS}
    ${Qt6Core_INCLUDE_DIRS}
    ${Qt6OpenGLWidgets_INCLUDE_DIRS}
)

# Link to this library
target_link_libraries(
    VoidRenderer
    PRIVATE
    VoidCore
    Qt6::Widgets
    Qt6::OpenGLWidgets
    OpenGL::GL
)
