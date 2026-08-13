set(SOURCES
    Delegates.cpp
    Dialog.cpp
    Frame.cpp
    Label.cpp
    LineEdit.cpp
    MessageBox.cpp
    ProgressTask.cpp
    PushButton.cpp
    Slider.cpp
    SpinBox.cpp
    Table.cpp
    Tooltip.cpp
    Window.cpp
)

add_library(
    VoidQExtensions
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidQExtensions
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidQExtensions
    PUBLIC
    VoidCore
    VoidIconForge
)

target_link_libraries(
    VoidQExtensions
    PRIVATE
    Qt::Core
    Qt::Widgets
)

# Export symbols
target_compile_definitions(VoidQExtensions PRIVATE VOID_LIB)
