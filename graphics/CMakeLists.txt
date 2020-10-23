
add_library(
    fundament_graphics
    STATIC
)

target_sources(
    fundament_graphics
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics.c
        ${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/graphics.h
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics_common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/private/graphics_common.h
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
)

add_library(
    fundament::graphics
    ALIAS
    fundament_graphics
)

set(FN_USE_VULKAN ON)

if(FN_USE_VULKAN)
    target_sources(
        fundament_graphics
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/private/vulkan/vulkan_graphics.c
            ${CMAKE_CURRENT_SOURCE_DIR}/private/vulkan/vulkan_graphics.h
    )

    target_link_libraries(
        fundament_graphics
        PUBLIC
            Vulkan::Vulkan
    )

    add_compile_definitions(
        fundament_graphics
        FN_USE_VULKAN
    )
endif()