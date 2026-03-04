set(SOURCES
    Core/Decoder.cpp
)

list(APPEND
    SOURCES

    Stream/Linux.cpp
)

find_package(ALSA REQUIRED)
find_package(PulseAudio REQUIRED)

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
    pulse
    pulse-simple
)

if (APPLE)
    target_link_libraries(VoidAudio PRIVATE swresample)
endif()

target_compile_definitions(VoidAudio PRIVATE VOID_LIB)
