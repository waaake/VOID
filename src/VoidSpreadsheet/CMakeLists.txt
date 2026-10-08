set(SOURCES

    PushButton.cpp
    Spreadsheet.cpp
    SpreadsheetWidget.cpp

    Delegates/ColorBoxDelegate.cpp
    Delegates/StatusDelegate.cpp

    Models/Spreadsheet.cpp

    Views/Spreadsheet.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidMediaPlayer/Player/Player.h
        ../VoidMediaPlayer/Player/PlayerBridge.h
        ../VoidMediaPlayer/Player/PlayerBuffer.h
        ../VoidMediaPlayer/Player/PlayerWidget.h
        ../VoidMediaPlayer/Project/Project.h
        ../VoidMediaPlayer/Player/ViewerBuffer.h
        ../VoidMediaPlayer/Media/MediaBridge.h
        ../VoidMenuSystem/MenuSystem.h
        ../VoidObjects/Core/Entity.h
        ../VoidObjects/Effects/Effects.h
        ../VoidObjects/Media/MediaClip.h
        ../VoidObjects/Models/EntityModel.h
        ../VoidObjects/Models/PlaylistModel.h
        ../VoidObjects/Models/TagModel.h
        ../VoidObjects/Models/TagMetadataModel.h
        ../VoidObjects/Playlist/Playlist.h
        ../VoidObjects/Project/Project.h
        ../VoidObjects/Sequence/Sequence.h
        ../VoidObjects/Sequence/Track.h
        ../VoidObjects/Sequence/TrackItem.h
        ../VoidQExtensions/Frame.h
        ../VoidQExtensions/Label.h
        ../VoidQExtensions/PushButton.h
        ../VoidQExtensions/Slider.h
        ../VoidTimeline/Timeline.h
        ../VoidTimeline/TimelineController.h
        ../VoidTimeline/Timeslider.h
        ../VoidSequencer/Models/SSelectionModel.h
    )
endif()

add_library(
    VoidSpreadsheet
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidSpreadsheet
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    ${FFMPEG_INCLUDE_DIRS} # This comes from AudioDecoder being included on the player that gets used when playerbridge is included; needs fixing
)

target_link_libraries(
    VoidSpreadsheet
    PUBLIC
    VoidCore
    VoidObjects
    VoidSequencer
    VoidMediaBrowser
    VoidMediaPlayer
)

target_link_libraries(
    VoidSpreadsheet
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidSpreadsheet PRIVATE VOID_LIB)
