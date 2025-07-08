# Build Render Library
add_library(
    VoidRenderer
    SHARED
    AnnotationRenderer.cpp
    Renderer.cpp
    RendererStatus.cpp
    RenderTypes.h
    VoidGL.cpp
    Programs/StrokeShaderProgram.cpp
    Gears/RenderGear.cpp
    Gears/StrokeRenderGear.cpp
)

target_include_directories(
    VoidRenderer
    PRIVATE
    include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QtWidgets_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
)

# Link to this library
target_link_libraries(
    VoidRenderer
    PRIVATE
    VoidCore
    Qt::Widgets
    OpenGL::GL
    glm::glm
    GLEW::GLEW
)

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VoidRenderer PRIVATE Qt::OpenGLWidgets)
endif()

# Export symbols for windows
target_compile_definitions(VoidRenderer PRIVATE VOID_LIB)
