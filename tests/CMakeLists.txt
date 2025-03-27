cmake_minimum_required(VERSION 3.2)
project(run_unit_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-ggdb -fprofile-arcs -ftest-coverage -g")

find_package(GTest REQUIRED)

add_subdirectory(../src ${CMAKE_BINARY_DIR}/sort_lib)

include(${CMAKE_CURRENT_LIST_DIR}/sort/sort_test.cmake)

add_test(run_unit_tests ${PROJECT_NAME})

# Source files
set(SOURCE_FILES
        pch.cpp
        test.cpp
)
set(HEADER_FILES
        pch.h
)

add_executable(
        ${PROJECT_NAME}
        ${HEADER_FILES}
        ${SOURCE_FILES}
        ${SORT_TEST}
)


# Precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)

target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)

target_include_directories(sort_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/../src)

target_link_libraries(
        ${PROJECT_NAME}
        sort_lib
        GTest::gtest
        GTest::gtest_main
)

enable_testing()
