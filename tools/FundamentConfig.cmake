cmake_minimum_required(VERSION 3.11)

add_library(fundament SHARED IMPORTED)

set(FUNDAMENT_OS)

set(_TARGET_OS "")
set(_LIB_NAME "")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(_TARGET_OS "linux")
    set(_LIB_NAME "libfundament.so")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(_TARGET_OS "darwin")
    set(_LIB_NAME "libfundament.dylib")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(_TARGET_OS "win32")
    set(_LIB_NAME "fundament.dll"
endif()

set(_ARTEFACT_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/bin/${_TARGET_OS}/debug/${_LIB_NAME})
set(_ARTEFACT_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/bin/${_TARGET_OS}/release/${_LIB_NAME})

target_include_directories(
    fundament
    INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/public
)

set_target_properties(
    fundament
    PROPERTIES
    IMPORTED_LOCATION_DEBUG ${_ARTEFACT_DEBUG} 
    IMPORTED_LOCATION_RELEASE ${_ARTEFACT_RELEASE} 
)


