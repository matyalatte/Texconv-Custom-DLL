cmake_minimum_required (VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
    GIT_SHALLOW TRUE
)

if (WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

file(GLOB_RECURSE TESTS test_*.cpp)
foreach(test ${TESTS})
    get_filename_component(target ${test} NAME_WE)
    add_executable(${target} ${test})
    target_link_libraries(${target} PUBLIC gtest_main texconv DirectXTex)
    gtest_discover_tests(${target}
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${target}>)
endforeach()

add_custom_command(
    TARGET test_non_dds POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/test.png" $<TARGET_FILE_DIR:test_non_dds>
)
