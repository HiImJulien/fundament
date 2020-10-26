
add_library(
    fundament_core
    STATIC
)

target_sources(
    fundament_core
    PRIVATE   
        ${CMAKE_CURRENT_SOURCE_DIR}/private/handle_pool.c
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/handle_pool.h
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/color.h
)

target_include_directories(
    fundament_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
)

add_library(
    fundament::core
    ALIAS
    fundament_core 
)

