set(SOURCES
    PreferencesUI.cpp

    CachePreferences.cpp
    GeneralPreferences.cpp
    MediaViewPreferences.cpp
    PlayerPreferences.cpp
)

# Windows needs the headers to be able to link against the meta object generated for the Q_OBJECT translations
if (WIN32)
    list(APPEND SOURCES
        ../VoidObjects/Preferences/Preferences.h
    )
endif()

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
