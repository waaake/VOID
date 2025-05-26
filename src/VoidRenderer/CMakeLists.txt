# Build Render Library
add_library(
    VoidRenderer
    SHARED
    Renderer.cpp
    RendererStatus.cpp
)

# Link to this library
target_link_libraries(
    VoidRenderer
    VoidCore
    Qt5::Widgets
    OpenGL::GL
)
