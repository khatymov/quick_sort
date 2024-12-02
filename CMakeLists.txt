cmake_minimum_required(VERSION 3.2)
project(my_project LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

include_directories(src)

add_executable(${PROJECT_NAME}
                main.cpp
)

# Precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src/pch.h)

target_link_libraries(${PROJECT_NAME} sort_lib)

target_compile_options(${PROJECT_NAME} PRIVATE -Werror -Wall -Wextra)



#add_subdirectory(tests)
