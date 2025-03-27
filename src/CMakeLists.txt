cmake_minimum_required(VERSION 3.2)
project(sort_lib)

# Set Release and Debug configurations
set(CMAKE_CONFIGURATION_TYPES "Release;Debug" CACHE STRING "Configurations" FORCE)

set(SOURCE_FILES
        pch.cpp
        timer.cpp
)

set(HEADER_FILES
        multithread_qsort.h
        multithread_partitioner.h
        pch.h
        sorter_interface.h
        standart_qsort.h
        simple_sort.h
        timer.h
)

# Define the executable target
add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)
