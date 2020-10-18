
add_executable(
    window_sample
    ${CMAKE_CURRENT_SOURCE_DIR}/window/main.c
)

target_link_libraries(
    window_sample
    PRIVATE
    fundament::window
)

add_executable(
    opengl_sample
    ${CMAKE_CURRENT_SOURCE_DIR}/opengl/main.c
)

target_link_libraries(
    opengl_sample
    PRIVATE 
    fundament::opengl
)