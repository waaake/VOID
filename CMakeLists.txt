cmake_minimum_required(VERSION 3.7.0)

project(VOID VERSION 0.0.1 LANGUAGES CXX)

# C++17 and Above
# Using std::filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

# USE CXX11 ABI
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=1)

# An explicit option to allow compiling the GUI frameless or with Window borders
# it's a debate about whether the feature should be inbuilt and an option to switch to either
# but the frameless way at the moment does not look really nice in mac when mac pulls the menubar away and we're
# only left with the icon on the left side and window buttons on the right
# for now, let's have it enabled on compile if we need it or not, later this might change and we have a full frameless
# or a framed window
option(USE_FRAMED_WINDOW "Enable using Framed Window borders, which is native themed." OFF)

# Mac has it turned on by default unless we don't want it there as well
if(APPLE)
    set(USE_FRAMED_WINDOW ON)
endif()

# Add Definition for the code to use the flag
if (USE_FRAMED_WINDOW)
    message(STATUS "Compiling with Framed Window")
    add_compile_definitions(USE_FRAMED_WINDOW)
endif()

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
    # Format c:\some\path\to\qt\cmake -> c:/some/path/to/qt/cmake
    file(TO_CMAKE_PATH ${Qt6_DIR} QT_CMAKE_PATH)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets Core)

    # Message to indicate we're falling back
    message(STATUS "Qt6 not found. Falling back to Qt5.")

    set(QT_MAJOR_VERSION 5)
    add_library(Qt::Core ALIAS Qt5::Core)
    add_library(Qt::Widgets ALIAS Qt5::Widgets)
    # Format c:\some\path\to\qt\cmake -> c:/some/path/to/qt/cmake
    file(TO_CMAKE_PATH ${Qt5_DIR} QT_CMAKE_PATH)
endif()

# Qt Dll copier for windows
if (WIN32)
    set(QT_DEPLOY_TOOL "${QT_CMAKE_PATH}/../../../bin/windeployqt.exe")
endif()

# GL
find_package(OpenGL REQUIRED COMPONENTS OpenGL)

# GL Functions
find_package(GLEW REQUIRED)

# GL Maths
find_package(glm REQUIRED)

# Freetype for Font Rendering
find_package(Freetype REQUIRED)

# Core Readers
find_package(OpenImageIO REQUIRED)
find_package(OpenEXR REQUIRED)
find_package(Imath REQUIRED)

# Color
find_package(OpenColorIO REQUIRED)

# Add Modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/Modules")

# Try and find ffmpeg
find_package(FFMPEG REQUIRED COMPONENTS avcodec avformat avutil swscale)

# Used for Logging
find_package(spdlog QUIET)

# Definitions
# Enable Logging if we have spdlog present and we're on the debug build
if (spdlog_FOUND AND CMAKE_BUILD_TYPE STREQUAL "debug")
    add_definitions(-DVOID_ENABLE_LOGGING)
endif()

# Add the VOID Source
add_subdirectory(src)
