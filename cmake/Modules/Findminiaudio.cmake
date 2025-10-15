
find_path(MINIAUDIO_INCLUDE_DIR
    NAMES miniaudio.h
    PATHS
        /usr/local/include/miniaudio
        /usr/include/miniaudio
)

find_library(MINIAUDIO_LIBRARY
    NAMES miniaudio
    PATHS
        /usr/local/lib/miniaudio
        /usr/lib/miniaudio
)

if(MINIAUDIO_INCLUDE_DIR)
    set(MINIAUDIO_FOUND TRUE)
    # add_library(miniaudio INTERFACE)
    # target_include_directories(miniaudio INTERFACE ${MINIAUDIO_INCLUDE_DIR})
else()
    set(MINIAUDIO_FOUND FALSE)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(miniaudio DEFAULT_MSG MINIAUDIO_INCLUDE_DIR)

if (MINIAUDIO_FOUND)
    add_library(miniaudio STATIC IMPORTED)
    set_target_properties(miniaudio PROPERTIES
        IMPORTED_LOCATION ${MINIAUDIO_LIBRARY}
        INTERFACE_INCLUDE_DIRECTORIES ${MINIAUDIO_INCLUDE_DIR}
    )
endif()
