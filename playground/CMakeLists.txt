# Defines targets for the playground.

set(SHADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/metal/shader/SimpleShader.metal"
	"${CMAKE_CURRENT_SOURCE_DIR}/metal/shader/VertexColorShader.metal"
	"${CMAKE_CURRENT_SOURCE_DIR}/metal/shader/Shader3Df3.metal"
)

set(SHADER_LIB
	${CMAKE_CURRENT_BINARY_DIR}/default.metallib
)

add_custom_command(
	OUTPUT ${SHADER_LIB}
	COMMAND xcrun -sdk macosx metal ${SHADERS} -o ${SHADER_LIB}
	DEPENDS ${SHADERS}
	COMMENT "Compiling metal shader."
)

add_executable(
	playground_metal_clear_screen
	${CMAKE_CURRENT_SOURCE_DIR}/metal/clear_screen/main.m
)

target_link_libraries(
	playground_metal_clear_screen
	PUBLIC
	fundament
)

add_executable(
	playground_metal_render_triangle
	${CMAKE_CURRENT_SOURCE_DIR}/metal/render_triangle/main.m
	${SHADER_LIB}
)

target_link_libraries(
	playground_metal_render_triangle
	PUBLIC
	fundament
)

set_target_properties(
	playground_metal_render_triangle
	PROPERTIES
	MACOSX_BUNDLE TRUE
	RESOURCE ${SHADER_LIB}
)

add_executable(
	playground_metal_colored_triangle
	${CMAKE_CURRENT_SOURCE_DIR}/metal/colored_triangle/main.m
	${SHADER_LIB}
)

target_link_libraries(
	playground_metal_colored_triangle
	PUBLIC
	fundament
)

set_target_properties(
	playground_metal_colored_triangle
	PROPERTIES
	MACOSX_BUNDLE TRUE
	RESOURCE ${SHADER_LIB}
)

add_executable(
	playground_metal_render_cube
	${CMAKE_CURRENT_SOURCE_DIR}/metal/render_cube/main.m
	${SHADER_LIB}
)

target_link_libraries(
	playground_metal_render_cube
	PUBLIC
	fundament
	"-framework Accelerate"
)

set_target_properties(
	playground_metal_render_cube
	PROPERTIES
	MACOSX_BUNDLE TRUE
	RESOURCE ${SHADER_LIB}
)
