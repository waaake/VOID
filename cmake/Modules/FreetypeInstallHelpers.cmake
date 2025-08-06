# Sets a few freetype paths in windows and more importantly sets the FREETYPE_DLL variable with
# the path to freetype.dll which is required by the player at runtime

if (WIN32 AND FREETYPE_FOUND)
    # Freetype DLL
    get_target_property(FREETYPE_LIB_PATH Freetype::Freetype LOCATION)
    # Get Parent dir path
    get_filename_component(FREETYPE_LIB_DIR ${FREETYPE_LIB_PATH} DIRECTORY) # ..
    get_filename_component(FREETYPE_DIR ${FREETYPE_LIB_DIR} DIRECTORY) # ..
    # Freetype BIN directory
    set(FREETYPE_BIN_DIR "${FREETYPE_DIR}/bin")

    # Find the freetype.dll from the BIN directory
    file(GLOB FREETYPE_DLL "${FREETYPE_BIN_DIR}/freetype*.dll")
endif()
