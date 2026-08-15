set(SOURCES
    Sequencer.cpp
    SController.cpp
    SMenu.cpp
    STimelineScene.cpp
    STimelineView.cpp
    STimelineGeometry.cpp

    Commands/SequenceCommands.cpp

    Graphics/SHandleItem.cpp
    Graphics/SPlayheadItem.cpp
    Graphics/SRazorItem.cpp
    Graphics/STimelineEffect.cpp
    Graphics/STimelineItem.cpp
    Graphics/STrack.cpp
    Graphics/STrackItem.cpp

    Models/SSelectionModel.cpp
    Models/SHoverModel.cpp

    Widgets/STimelineRuler.cpp
    Widgets/STrackHeader.cpp
    Widgets/STrackHeaderWidget.cpp
    Widgets/SToolbar.cpp
)

add_library(
    VoidSequencer
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidSequencer
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidSequencer
    PUBLIC
    VoidCore
    VoidMediaPlayer # Needs to be fixed when bridges are separate entities
    VoidObjects
    VoidQExtensions
    VoidTimeline
    VoidUndo
)

target_link_libraries(
    VoidSequencer
    PRIVATE
    Qt::Core
    Qt::Widgets
)

target_compile_definitions(VoidSequencer PRIVATE VOID_LIB)
