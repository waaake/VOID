# Build Render Library
add_library(
    VoidRenderer
    SHARED
    Renderer.cpp
    RendererStatus.cpp
    RenderTypes.h
    VoidRenderer.cpp

    # Render Components (Gears)
    Gears/RenderGear.cpp
    Gears/ImageRenderGear.cpp
    Gears/ImageComparisonRenderGear.cpp
    Gears/StrokeRenderGear.cpp
    Gears/SwiperRenderGear.cpp
    Gears/TextRenderGear.cpp

    # Render Layers
    Layers/ImageRenderLayer.cpp
    Layers/ImageComparisonRenderLayer.cpp
    Layers/AnnotationRenderLayer.cpp

    # Shader Programs
    Programs/ImageShaderProgram.cpp
    Programs/ImageComparisonShaderProgram.cpp
    Programs/StrokeShaderProgram.cpp
    Programs/SwiperShaderProgram.cpp
    Programs/TextShaderProgram.cpp
)

target_include_directories(
    VoidRenderer
    PRIVATE
    include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QtWidgets_INCLUDE_DIRS}
    ${glm_INCLUDE_DIRS}
    ${FREETYPE_INCLUDE_DIRS}
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
    Freetype::Freetype
)

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VoidRenderer PRIVATE Qt::OpenGLWidgets)
endif()

# Export symbols for windows
target_compile_definitions(VoidRenderer PRIVATE VOID_LIB)
