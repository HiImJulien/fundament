
add_executable(
    simple_window
    ${CMAKE_CURRENT_SOURCE_DIR}/simple_window.c
)

target_link_libraries(
    simple_window
    PRIVATE
    fundament
)

