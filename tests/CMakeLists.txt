cmake_minimum_required(VERSION 3.2)
project(run_unit_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-ggdb -fprofile-arcs -ftest-coverage")

SET(CMAKE_CXX_FLAGS "-g ${CMAKE_CXX_FLAGS}")

find_package(GTest REQUIRED)

include_directories(../src)

include(${CMAKE_CURRENT_LIST_DIR}/my_class/my_class_test.cmake)

add_executable(
        ${PROJECT_NAME}
        test.cpp
        ${MY_CLASS_TEST}
)

# Precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../src/pch.h)

target_link_libraries(
        ${PROJECT_NAME}
        sort_lib
        gtest
)

enable_testing()

add_test(run_unit_tests ${PROJECT_NAME})