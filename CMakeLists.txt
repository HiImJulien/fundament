cmake_minimum_required(VERSION 3.7)
project(fundament
    VERSION 0.4
    DESCRIPTION "A slick foundation for game developers."
    HOMEPAGE_URL "https://github.com/HiImJulien/fundament"
)

option(BUILD_SAMPLES "Whether to build samples or not.")

set(FN_TARGET_IS_MACOS $<STREQUAL:${CMAKE_SYSTEM_NAME},"Darwin">)
set(FN_TARGET_IS_WIN32 $<STREQUAL:${CMAKE_SYSTEM_NAME},"Windows">)
set(FN_TARGET_IS_LINUX $<STREQUAL:${CMAKE_SYSTEM_NAME},"Linux">)

find_package(Vulkan)

add_subdirectory(core)
add_subdirectory(window)
add_subdirectory(graphics)

if(NOT FN_TARGET_IS_MACOS)
    add_subdirectory(opengl)
endif()

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
