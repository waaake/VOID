set(SOURCES
    Core/AudioStream.cpp
)

# if
list(APPEND SOURCES
    Platform/LinuxStream.cpp
)

find_package(ALSA REQUIRED)
find_package(PulseAudio REQUIRED)

add_library(VoidAudio
    SHARED
    ${SOURCES}    
)

target_link_libraries(VoidAudio
    PUBLIC
    VoidCore
)

target_link_libraries(VoidAudio
    PRIVATE
    pulse
    pulse-simple
)

target_compile_definitions(VoidAudio PRIVATE VOID_LIB)
