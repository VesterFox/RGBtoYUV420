project(Google_tests)

add_subdirectory(lib)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(TEST_DIR ${PROJECT_SOURCE_DIR})
set(TEST_SOURCES
        ${TEST_DIR}/utils_tests.cpp
        ${TEST_DIR}/bmp_tests.cpp
        ${TEST_DIR}/yuv_tests.cpp
)

add_executable(Google_Tests_run ${TEST_SOURCES})

target_link_libraries(Google_Tests_run gtest gtest_main RGBtoYUV420_lib)