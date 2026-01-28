# Build Render Library
add_library(
    VoidRenderer
    SHARED
    Renderer.h
    Renderer.cpp
    RendererStatus.cpp
    VoidRenderer.h
    VoidRenderer.cpp

    # Core
    Core/FontAtlas.cpp
    Core/FontEngine.cpp
    Core/RenderTypes.cpp

    # Render Layers
    Layers/ImageRenderLayer.cpp
    Layers/ImageComparisonRenderLayer.cpp
    Layers/StrokeRenderLayer.cpp
    Layers/SwipeRenderLayer.cpp
    Layers/TextRenderLayer.cpp

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
    Qt::Core
    OpenGL::GL
    glm::glm
    GLEW::GLEW
    Freetype::Freetype
)

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VoidRenderer PRIVATE Qt::OpenGLWidgets)
endif()

# Mac for some reason needs this explicitly
# TODO: Check if we can statically link OCIO to VoidCore?
if (APPLE)
    target_link_libraries(VoidRenderer PRIVATE OpenColorIO::OpenColorIO)
endif()

# Export symbols for windows
target_compile_definitions(VoidRenderer PRIVATE VOID_LIB)
