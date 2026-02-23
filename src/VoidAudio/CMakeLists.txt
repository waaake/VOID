
add_library(
    VoidAudio
    SHARED
    AudioStream.cpp
)

target_link_libraries(
    VoidAudio
    PUBLIC
    VoidCore
)

# target_include_directories(
#     VoidAudio
#     PRIVATE

#     ${CMAKE_CURRENT_SOURCE_DIR}/../../
# )

# Export symbols for windows
target_compile_definitions(VoidAudio PRIVATE VOID_LIB)
