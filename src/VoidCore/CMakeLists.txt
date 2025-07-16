
# Find Boost for uuid
find_package(Boost REQUIRED)

# Build Core Library
add_library(
    VoidCore
    SHARED
    FormatForge.cpp
    Identity.h
    ImageData.cpp
    Logging.cpp
    Media.cpp
    MediaFilesystem.cpp
    VoidTools.cpp
    # Media Readers
    Readers/OIIOReader.cpp
    Readers/OpenEXRReader.cpp
    Readers/FFmpegReader.cpp
)

target_include_directories(
    VoidCore
    PRIVATE
    ${Boost_INCLUDE_DIRS}
    ${FFMPEG_INCLUDE_DIRS}
)

# Link to this library
target_link_libraries(
    VoidCore
    PRIVATE
    Imath::Imath
    OpenEXR::OpenEXR
    OpenImageIO::OpenImageIO
    ${FFMPEG_LIBRARIES}
)

# Mac for some reason needs this explicitly
if (APPLE)
    target_link_libraries(VoidCore PRIVATE swscale)
endif()

# Export symbols for windows
target_compile_definitions(VoidCore PRIVATE VOID_LIB)
