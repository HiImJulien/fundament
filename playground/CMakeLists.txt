# Defines targets for the playground.

add_executable(
	playground_metal_clear_screen
	${CMAKE_CURRENT_SOURCE_DIR}/metal/clear_screen/main.m
)

target_link_libraries(
	playground_metal_clear_screen
	PUBLIC
	fundament
)
