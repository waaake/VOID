set(SOURCES

    ColorEngine.cpp
)

add_library(
    VoidColor
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidColor
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidColor
    PUBLIC
    VoidCore
    VoidObjects
)

target_link_libraries(
    VoidColor
    PRIVATE
    Qt::Core
    OpenColorIO::OpenColorIO
)

# export
target_compile_definitions(VoidColor PRIVATE VOID_LIB)
