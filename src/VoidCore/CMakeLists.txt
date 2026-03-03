
# Find Boost for uuid
find_package(Boost REQUIRED)

# Build Core Library
set(SOURCES
    ColorProcessor.cpp
    FormatForge.cpp
    Identity.h
    ImageData.cpp
    Logging.cpp
    Profiler.h
    PyExecutor.cpp
    VoidTools.cpp

    # Plugins
    Plugins/Loader.cpp

    # Media
    Media/Filesystem.cpp
    Media/Frame.cpp
    Media/Media.cpp

    # Media Readers
    Readers/OIIOReader.cpp
    Readers/OpenEXRReader.cpp
    Readers/FFmpegReader.cpp
    Readers/TurboJpegReader.cpp
)

add_library(
    VoidCore
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidCore
    PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
    ${OpenColorIO_INCLUDE_DIRS}
    ${TurboJPEG_INCLUDE_DIRS}
)

# Link to this library
target_link_libraries(
    VoidCore
    PRIVATE
    Imath::Imath
    OpenEXR::OpenEXR
    OpenImageIO::OpenImageIO
    ${FFMPEG_LIBRARIES}
    OpenColorIO::OpenColorIO
    pybind11::embed
    Python::Python
    ${TurboJPEG_LIBRARIES}
)

# Mac for some reason needs this explicitly
if (APPLE)
    target_link_libraries(VoidCore PRIVATE swscale swresample)
endif()

# Export symbols for windows
target_compile_definitions(VoidCore PRIVATE VOID_LIB LOGGER_LIB)
