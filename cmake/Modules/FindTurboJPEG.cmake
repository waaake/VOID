# Finds and Sets up Jpeg Library Components
find_path(TurboJPEG_INCLUDE_DIR turbojpeg.h)
find_library(TurboJPEG_LIBRARY turbojpeg)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TurboJPEG DEFAULT_MSG TurboJPEG_LIBRARY TurboJPEG_INCLUDE_DIR)

if (TurboJPEG_FOUND)
    set(TurboJPEG_LIBRARIES ${TurboJPEG_LIBRARY})
    set(TurboJPEG_INCLUDE_DIRS ${TurboJPEG_INCLUDE_DIR})
endif()
