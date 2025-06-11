
# Find Boost for uuid
find_package(Boost REQUIRED)

# Build Core Library
add_library(
    VoidCore
    SHARED
    Identity.h
    ImageData.cpp
    Logging.cpp
    Media.cpp
    VoidTools.cpp
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
