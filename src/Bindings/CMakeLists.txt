# Python Bindings
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Sources
set(SOURCES
    Module.cpp
    CoreModule.cpp
    UiModule.cpp
)

# Create Python Module
pybind11_add_module(voidpy "${SOURCES}")

target_link_libraries(
    voidpy
    PRIVATE
    VoidCore
    VoidUi
    Qt::Widgets
    Freetype::Freetype
)

# Qt6 needs OpenGLWidgets be included/linked separately
if (QT_MAJOR_VERSION EQUAL 6)
    target_link_libraries(voidpy PRIVATE Qt::OpenGLWidgets)
endif()
