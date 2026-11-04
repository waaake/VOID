set(SOURCES
    MenuSystem.cpp
)

add_library(
    VoidMenuSystem
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidMenuSystem
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidMenuSystem
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidMenuSystem PRIVATE VOID_LIB)
