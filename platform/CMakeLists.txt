
add_library(
    fundament_window
    STATIC
)

target_sources(
    fundament_window
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window.c
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/window.h
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/window_common.h
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

