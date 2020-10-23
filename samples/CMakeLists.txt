
add_executable(
    graphics_sample
    ${CMAKE_CURRENT_SOURCE_DIR}/graphics/main.c
)

target_link_libraries(
    graphics_sample
    PRIVATE
        fundament::window
        fundament::graphics
)

# fundament::opengl is not available on macOS
if(TARGET fundament::opengl)
    add_executable(
        opengl_sample
        ${CMAKE_CURRENT_SOURCE_DIR}/opengl/main.c
    )

    target_link_libraries(
        opengl_sample
        PRIVATE 
        fundament::opengl
    )
endif()

add_executable(
    window_sample
    ${CMAKE_CURRENT_SOURCE_DIR}/window/main.c
)

target_link_libraries(
    window_sample
    PRIVATE
    fundament::window
)
