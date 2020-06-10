cmake_minimum_required(VERSION 3.11)
project(fundament)

option(BUILD_SAMPLES "Build the samples" FALSE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/c)

if(${BUILD_SAMPLES})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/samples/c)
endif()

