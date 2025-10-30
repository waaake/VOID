
find_path(MINIAUDIO_INCLUDE_DIR
    NAMES miniaudio.h
    PATH_SUFFIXES miniaudio
)

find_library(MINIAUDIO_LIBRARY
    NAMES miniaudio
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(miniaudio
    REQUIRED_VARS MINIAUDIO_INCLUDE_DIR
    HANDLE_COMPONENTS
)

if (MINIAUDIO_INCLUDE_DIR AND MINIAUDIO_LIBRARY)
    # Compiled Version of the library
    add_library(miniaudio STATIC IMPORTED)
    set_target_properties(miniaudio PROPERTIES
        IMPORTED_LOCATION "${MINIAUDIO_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${MINIAUDIO_INCLUDE_DIR}"
    )
    set(MINIAUDIO_IS_HEADER_ONLY FALSE)
elseif (MINIAUDIO_INCLUDE_DIR)
    # Header-only version
    add_library(miniaudio INTERFACE)
    target_include_directories(miniaudio INTERFACE "${MINIAUDIO_INCLUDE_DIR}")
    set(MINIAUDIO_IS_HEADER_ONLY TRUE)
else()
    set(MINIAUDIO_FOUND FALSE)
endif()
