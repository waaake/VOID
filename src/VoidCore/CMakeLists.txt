
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
    VoidTools.cpp
    # Readers
    Readers/OIIOReader.cpp
)

target_include_directories(
    VoidCore
    PRIVATE
    ${Boost_INCLUDE_DIRS}
)

# Link to this library
target_link_libraries(
    VoidCore
    PRIVATE
    Imath::Imath
    OpenEXR::OpenEXR
    OpenImageIO::OpenImageIO
)

# Export symbols for windows
target_compile_definitions(VoidCore PRIVATE VOID_LIB)
