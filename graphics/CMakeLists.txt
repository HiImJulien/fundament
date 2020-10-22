
add_library(
    fundament_graphics
    STATIC
)

target_sources(
    fundament_graphics
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics.c
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/graphics.h
)

target_include_directories(
    fundament_graphics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
)

target_link_libraries(
    fundament_graphics
    PUBLIC
        fundament::core
        Vulkan::Vulkan
)

add_library(
    fundament::graphics
    ALIAS
    fundament_graphics
)
