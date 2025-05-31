# Build Core Library
add_library(
    VoidCore
    SHARED
    ImageData.cpp
    Logging.cpp
    Media.cpp
    VoidTools.cpp
)

# Link to this library
target_link_libraries(
    VoidCore
    PRIVATE
    OpenImageIO::OpenImageIO
)
