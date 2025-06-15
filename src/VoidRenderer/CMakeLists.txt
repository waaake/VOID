# Build Render Library
add_library(
    VoidRenderer
    SHARED
    Renderer.cpp
    RendererStatus.cpp
    VoidGL.cpp
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
)

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(VoidRenderer PRIVATE Qt::OpenGLWidgets)
endif()
