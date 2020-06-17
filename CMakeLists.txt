cmake_minimum_required(VERSION 3.17)
project(fundament VERSION 0.2.0)

option(BUILD_SAMPLES "Build the samples" FALSE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/c)

if(${BUILD_SAMPLES})
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/samples/c)
endif()

set(CPACK_PACKAGE_VENDOR "Julian Kirsch")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A slick foundation for game developers.")
set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_IGNORE_FILES
    /.git
    /build
)

include(CPack) 
