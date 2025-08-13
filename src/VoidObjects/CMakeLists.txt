# Build Intermediate Objects Library
set(SOURCES
    Commands/VoidCommand.cpp
    Commands/MediaCommands.cpp

    Media/MediaBridge.cpp
    Media/MediaClip.cpp

    Models/MediaModel.cpp
    Models/ProjectModel.cpp

    Project/Project.cpp

    Sequence/Sequence.cpp
    Sequence/Track.cpp
    Sequence/TrackItem.cpp
)

add_library(
    VoidObjects
    SHARED
    ${SOURCES}
)

target_link_libraries(
    VoidObjects
    PUBLIC
    VoidCore
    VoidRenderer
)

# Link to this library
target_link_libraries(
    VoidObjects
    PRIVATE
    Qt::Core

    # Media bridge relies on undo commands and stack, which are GUI entities
    # TODO: Strip out core Bridge from Media bridge which can be in the GUI lib
    Qt::Gui
    Qt::Widgets
)

# Export symbols for windows
target_compile_definitions(VoidObjects PRIVATE VOID_LIB)
