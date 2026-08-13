set(SOURCES
    ScriptConsole.cpp
    ScriptEditor.cpp
    SyntaxHighlighter.cpp
)

add_library(
    VoidScriptEditor
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidScriptEditor
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidScriptEditor
    PUBLIC
    VoidCore
    VoidObjects
    VoidQExtensions
)

target_link_libraries(
    VoidScriptEditor
    PRIVATE
    Qt::Core
    Qt::Widgets
    Python::Python
)

# Export Symbols
target_compile_definitions(VoidScriptEditor PRIVATE VOID_LIB)
