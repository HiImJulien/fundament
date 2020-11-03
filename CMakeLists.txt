cmake_minimum_required(VERSION 3.7)
project(fundament
    VERSION 0.4
    DESCRIPTION "A slick foundation for game developers."
    HOMEPAGE_URL "https://github.com/HiImJulien/fundament"
)

option(BUILD_SAMPLES "Whether to build samples or not.")

if(UNIX AND APPLE)
    set(FN_TARGET_IS_MACOS YES)
elseif(WIN32)
    set(FN_TARGET_IS_WIN32 YES)
elseif(UNIX)
    set(FN_TARGET_IS_LINUX YES)
else()
    message(FATAL_ERROR Unknown target platform!)
endif()

find_package(Vulkan)
find_package(PkgConfig)

add_subdirectory(core)
add_subdirectory(window)
add_subdirectory(graphics)

if(NOT FN_TARGET_IS_MACOS)
    add_subdirectory(opengl)
endif()

if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
