set(SOURCES

    Spreadsheet.cpp
    SpreadsheetWidget.cpp

    Models/Spreadsheet.cpp
    Views/Spreadsheet.cpp
)

add_library(
    VoidSpreadsheet
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidSpreadsheet
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidSpreadsheet
    PUBLIC
    VoidCore
    VoidObjects
    VoidSequencer
)

target_link_libraries(
    VoidSpreadsheet
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidSpreadsheet PRIVATE VOID_LIB)
