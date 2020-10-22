cmake_minimum_required(VERSION 3.7)
project(fundament
    VERSION 0.4
    DESCRIPTION "A slick foundation for game developers."
    HOMEPAGE_URL "https://github.com/HiImJulien/fundament"
)

option(BUILD_SAMPLES "Whether to build samples or not.")

find_package(Vulkan)

add_subdirectory(core)
add_subdirectory(opengl)
add_subdirectory(window)
add_subdirectory(graphics)

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
