# Finds and Sets up Jpeg Library Components
find_path(TurboJPEG_INCLUDE_DIR turbojpeg.h)
find_library(TurboJPEG_LIBRARY turbojpeg)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(TurboJPEG DEFAULT_MSG TurboJPEG_LIBRARY TurboJPEG_INCLUDE_DIR)

if (TurboJPEG_FOUND)
    set(TurboJPEG_LIBRARIES ${TurboJPEG_LIBRARY})
    set(TurboJPEG_INCLUDE_DIRS ${TurboJPEG_INCLUDE_DIR})
endif()

if (WIN32 AND TurboJPEG_FOUND)
    get_filename_component(TurboJPEG_LIB_DIR ${TurboJPEG_LIBRARY} DIRECTORY) # ..
    get_filename_component(TurboJPEG_DIR ${TurboJPEG_LIB_DIR} DIRECTORY) # ..
    
    # Bin Directory
    set(TurboJPEG_BIN_DIR "${TurboJPEG_DIR}/bin")

    # Find turbojpeg.dll from the bin directory
    file(GLOB TurboJPEG_DLL "${TurboJPEG_BIN_DIR}/turbojpeg*.dll")
endif()
