set(SOURCES
    Core/Decoder.cpp
)

if (WIN32)
list(APPEND SOURCES Stream/Win.cpp)
elseif (APPLE)
list(APPEND SOURCES Stream/Mac.cpp)
elseif (UNIX)
list(APPEND SOURCES Stream/Linux.cpp)

find_package(ALSA REQUIRED)
find_package(PulseAudio REQUIRED)
endif()


add_library(
    VoidAudio
    SHARED
    ${SOURCES}
)

target_link_libraries(
    VoidAudio
    PUBLIC
    VoidCore
    # VoidObjects
)

target_link_libraries(
    VoidAudio
    PRIVATE
    ${FFMPEG_LIBRARIES}
)

target_include_directories(
    VoidAudio
    PRIVATE
    ${FFMPEG_INCLUDE_DIRS}
)

if (APPLE)
target_link_libraries(
    VoidAudio
    PRIVATE
    "-framework CoreFoundation" "-framework AudioToolbox"
)
elseif (UNIX)
target_link_libraries(
    VoidAudio
    PRIVATE
    pulse
    pulse-simple
)
endif()

if (APPLE)
    target_link_libraries(VoidAudio PRIVATE swresample)
endif()

target_compile_definitions(VoidAudio PRIVATE VOID_LIB)
