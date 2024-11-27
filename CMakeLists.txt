cmake_minimum_required(VERSION 3.2)
project(my_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

add_executable(${PROJECT_NAME}
                main.cpp
)

add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} sort_lib)

target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)

add_subdirectory(tests)
