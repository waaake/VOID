set(SOURCES
    Engine.cpp
)

add_library(
    VoidBridge
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidBridge
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidBridge
    PRIVATE
    VoidObjects
)

target_link_libraries(
    VoidBridge
    PRIVATE
    Qt::Core
)

target_compile_definitions(VoidBridge PRIVATE VOID_LIB)
