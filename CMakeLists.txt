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

# Core Depends on Qt5/Qt6 | OpenGL
# Supporting both Qt5 and Qt6 at the moment as they are mostly compatible with minor changes
# If we're at a point where the backwards compatibility is too much of a hassle, we could then drop support for qt5 but
# for now we're good

# Ensure we have atleast both of them present
# Find qt6 first
find_package(Qt6 QUIET COMPONENTS Widgets Core OpenGLWidgets)
if (Qt6_FOUND)
    set(QT_MAJOR_VERSION 6)
    add_library(Qt::Core ALIAS Qt6::Core)
    add_library(Qt::Widgets ALIAS Qt6::Widgets)
    add_library(Qt::OpenGLWidgets ALIAS Qt6::OpenGLWidgets)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Core)

    # Message to indicate we're falling back
    message(STATUS "Qt6 not found. Falling back to Qt5.")

    set(QT_MAJOR_VERSION 5)
    add_library(Qt::Core ALIAS Qt5::Core)
    add_library(Qt::Widgets ALIAS Qt5::Widgets)
endif()
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
