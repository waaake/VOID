cmake_minimum_required(VERSION 3.5.0)

project(VOID VERSION 0.0.1 LANGUAGES CXX)

# C++17 and Above
# Using std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

if (CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

# USE CXX11 ABI
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

# Core Depends on Qt5 | OpenGL
# Ensure we have atleast both of them present
find_package(Qt5 REQUIRED COMPONENTS Widgets Core)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

# Core Readers
find_package(OpenImageIO REQUIRED)
find_package(OpenEXR REQUIRED)
find_package(Imath REQUIRED)

# Used for Logging
find_package(spdlog QUIET)

# Definitions
# Enable Logging if we have spdlog present and we're on the debug build
if (spdlog_FOUND AND CMAKE_BUILD_TYPE STREQUAL "debug")
    add_definitions(-DVOID_ENABLE_LOGGING)
endif()

# Add the VOID Source
add_subdirectory(src)
