set(SOURCES
    # Editors
    Editor/Effects.cpp
    Editor/ParamEditor.cpp
    Editor/Properties.cpp
    Editor/RangedParamEditor.cpp

    Tools/Models/SnapshotModel.cpp

    # Tools
    Tools/Delegates/LogDelegate.cpp
    Tools/SnapshotBox.cpp
    Tools/Task.cpp
    Tools/TaskView.cpp
    Tools/LogWindow.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidObjects/Effects/Effects.h
        ../VoidObjects/Core/Task.h
        ../VoidQExtensions/Frame.h
        ../VoidQExtensions/LineEdit.h
        ../VoidQExtensions/PushButton.h
        ../VoidQExtensions/Slider.h
        ../VoidQExtensions/Table.h
    )
endif()

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
