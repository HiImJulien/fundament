
add_executable(
    window_sample
    ${CMAKE_CURRENT_SOURCE_DIR}/window/main.c
)

target_link_libraries(
    window_sample
    PRIVATE
    fundament::window
)

