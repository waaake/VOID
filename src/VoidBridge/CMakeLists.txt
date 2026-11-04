set(SOURCES
    Engine.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidObjects/Core/Task.h
    )
endif()

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
