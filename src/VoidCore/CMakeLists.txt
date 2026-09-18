
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
    Timecode.cpp
    Timekeeper.cpp
    VoidTools.cpp

    # Plugins
    Plugins/Loader.cpp

    # Media
    Media/Filesystem.cpp
    Media/Frame.cpp
    Media/Media.cpp
    Media/Renderer.cpp
    Media/Tokens.cpp

    # Media Readers
    Readers/OIIOReader.cpp
    Readers/OpenEXRReader.cpp
    Readers/FFmpegReader.cpp
    Readers/TurboJpegReader.cpp

    # Operators
    Operators/Grader.cpp
    Operators/Flipper.cpp
    Operators/Inverter.cpp
    Operators/Operator.cpp
    Operators/Param.cpp

    # Processors
    Processors/ImageProcessor.cpp

    # Media Writers
    Writers/OIIOWriter.cpp
    Writers/FFmpegWriter.cpp
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
    TBB::tbb
)

# Mac for some reason needs this explicitly
if (APPLE)
    target_link_libraries(VoidCore PRIVATE swscale swresample)
endif()

# Export symbols for windows
target_compile_definitions(VoidCore PRIVATE VOID_LIB LOGGER_LIB)

# NOTE: this for now has been resolved by directly using tbb for the parallel execution
# We no longer use an internal c++ std exeuction policy to generate that code for us
# But this needs to be better understood, or probably we better handle the reader --- for later

# # Apple clang by default does not allow using std::execution::par
# # This however is set to be enabled with the _LIBCPP_HAS_EXPERIMENTAL_PSTL define
# # for now keeping it simple and going with sequential
# if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#     target_compile_definitions(VoidCore PRIVATE _DISABLE_PARALLEL_EXECUTION)
# endif()
