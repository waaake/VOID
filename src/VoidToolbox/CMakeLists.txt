set(SOURCES
    # Editors
    Editor/Effects.cpp
    Editor/ParamEditor.cpp
    Editor/Properties.cpp
    Editor/RangedParamEditor.cpp

    # Tools
    Tools/Delegates/LogDelegate.cpp
    Tools/Task.cpp
    Tools/TaskView.cpp
    Tools/LogWindow.cpp
)

add_library(
    VoidToolbox
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidToolbox
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidToolbox
    PUBLIC
    VoidCore
    VoidObjects
    VoidQExtensions
)

target_link_libraries(
    VoidToolbox
    PRIVATE
    Qt::Core
    Qt::Widgets
)

# Export symbols
target_compile_definitions(VoidToolbox PRIVATE VOID_LIB)
