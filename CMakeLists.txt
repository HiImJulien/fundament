cmake_minimum_required(VERSION 3.1)
project(fundament
    VERSION 0.4
    DESCRIPTION "A slick foundation for game developers."
    HOMEPAGE_URL "https://github.com/HiImJulien/fundament"
)

option(BUILD_SAMPLES "Whether to build samples or not.")

add_subdirectory(core)
add_subdirectory(opengl)
add_subdirectory(window)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
