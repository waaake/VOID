set(SOURCES
    PreferencesUI.cpp

    CachePreferences.cpp
    GeneralPreferences.cpp
    MediaViewPreferences.cpp
    PlayerPreferences.cpp
)

add_library(
    VoidPreferences
    SHARED
    ${SOURCES}
)

target_include_directories(
    VoidPreferences
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(
    VoidPreferences
    PUBLIC
    VoidCore
    VoidObjects
)

target_link_libraries(
    VoidPreferences
    PRIVATE
    Qt::Core
    Qt::Widgets
)

# Export symbols
target_compile_definitions(VoidPreferences PRIVATE VOID_LIB)
