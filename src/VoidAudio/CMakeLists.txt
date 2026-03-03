set(SOURCES
    Core/Stream.cpp
)

add_library(
    VoidAudio
    SHARED
    ${SOURCES}
)

target_link_libraries(
    VoidAudio
    PUBLIC
    VoidCore
    VoidObjects
)

target_link_libraries(
    VoidAudio
    PRIVATE
    ${FFMPEG_LIBRARIES}
)

if (APPLE)
    target_link_libraries(VoidAudio PRIVATE swresample)
endif()

target_compile_definitions(VoidAudio PRIVATE VOID_LIB)
