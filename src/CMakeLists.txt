cmake_minimum_required(VERSION 3.19)
project(sort_lib)

set(SOURCE_FILES
        timer.cpp
)

set(HEADER_FILES
        timer.h
        sorter_interface.h
        standart_qsort.h
        simple_sort.h
        multithread_qsort.h
        multithread_partitioner.h
)

# Define the executable target
add_library(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Precompiled header
target_precompile_headers(${PROJECT_NAME} PRIVATE pch.h)
