
add_library(
    fundament_window
    STATIC
)

target_sources(
    fundament_window
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/input.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window_common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/input.h
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/window.h
)

target_include_directories(
    fundament_window
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
)

target_link_libraries(
    fundament_window
    PUBLIC
        fundament::core
)

add_library(
    fundament::window
    ALIAS
    fundament_window
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(
        fundament_window
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/win32/win32_window.c
            ${CMAKE_CURRENT_SOURCE_DIR}/private/win32/win32_window.h
    )

    target_link_libraries(
        fundament_window
        PUBLIC
            user32
    )
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_sources(
        fundament_window
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/cocoa/cocoa_window.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/cocoa/cocoa_window.m
            ${CMAKE_CURRENT_SOURCE_DIR}/private/cocoa/cocoa_window_delegate.h
            ${CMAKE_CURRENT_SOURCE_DIR}/private/cocoa/cocoa_window_delegate.m
    )

    target_link_libraries(
        fundament_window
        PUBLIC
        "-framework AppKit"
    )
    
    target_compile_features(
        fundament_window
        PRIVATE
            c_std_11
    )
    
endif()

