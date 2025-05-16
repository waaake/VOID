# Build Core Library
add_subdirectory(VoidCore)

# Build Renderer Library
add_subdirectory(VoidRenderer)

# Build Ui Library
add_subdirectory(VoidUi)

# Depends on Qt
target_include_directories(
    VoidUi
    PUBLIC
    include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Core_INCLUDE_DIRS}
)

target_include_directories(
    VoidCore
    PUBLIC
    include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../extern
    )

# Logging
if (spdlog_FOUND)
    target_include_directories(
        VoidCore
        PRIVATE
        ${spdlog_INCLUDE_DIRS}
    )
endif()

# Main Executable
add_executable(
    VOID
    Application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../extern/stb_image.cpp
)

# Finally Linking to our executable
target_link_libraries(
    VOID
    VoidCore
    VoidRenderer
    VoidUi
    Qt5::Widgets
    Qt5::Core
    OpenGL::GL
)
