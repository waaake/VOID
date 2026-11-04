set(SOURCES

    Delegates/VersionDelegate.cpp

    VersionView.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidObjects/Core/Entity.h
        ../VoidObjects/Media/MediaClip.h
        ../VoidObjects/Models/EntityModel.h
        ../VoidObjects/Models/TagModel.h
        ../VoidObjects/Models/TagMetadataModel.h
        ../VoidObjects/Sequence/Sequence.h
        ../VoidObjects/Sequence/Track.h
        ../VoidObjects/Sequence/TrackItem.h
    )
endif()

add_library(
    VoidViews
    SHARED

    ${SOURCES}
)

target_include_directories(
    VoidViews
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidViews
    PUBLIC
    VoidCore
    VoidObjects
)

target_link_libraries(
    VoidViews
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidViews PRIVATE VOID_LIB)
