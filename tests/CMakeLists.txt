include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.zip
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(void_test
    Timecode_tests.cpp
    FrameRange_tests.cpp
)

target_link_libraries(
    void_test
    PRIVATE
    VoidCore
    GTest::gtest_main
)

target_include_directories(
    void_test
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../src>
)

include(GoogleTest)
gtest_discover_tests(void_test)
