#
# Specifies how to build the main library.
#

add_library(
    fundament
    SHARED
    ""
)

target_sources(
    fundament
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/input.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/input_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window_common.c
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/api.h
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/event.h
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/input.h
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/window.h
)

target_include_directories(
    fundament
    PUBLIC
        $<INSTALL_INTERFACE:public>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/public>
)

target_compile_definitions(
    fundament
    PUBLIC
    c_std_11
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    target_sources(
        fundament
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/input_win32.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/input_win32.c
            ${CMAKE_CURRENT_SOURCE_DIR}/private/window_win32.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/window_win32.c
    )

    target_link_libraries(
        fundament
        PRIVATE
        user32
    )
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    target_sources(
        fundament
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/input_AppKit.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/input_AppKit.m
            ${CMAKE_CURRENT_SOURCE_DIR}/private/window_AppKit.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/window_AppKit.m
    )
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    target_sources(
        fundament
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/input_Xcb.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/input_Xcb.c
            ${CMAKE_CURRENT_SOURCE_DIR}/private/window_Xcb.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/window_Xcb.c
    )

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Xcb REQUIRED IMPORTED_TARGET xcb)

    target_link_libraries(
        fundament
        PRIVATE
        PkgConfig::Xcb
    )
endif()

get_target_property(PUBLIC_HEADERS fundament INTERFACE_SOURCES)
set_target_properties(
    fundament
    PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

install(
    TARGETS fundament 
    EXPORT fundament
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION public
    PUBLIC_HEADER DESTINATION public/fundament
)
