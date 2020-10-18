
add_library(
	fundament_opengl
	STATIC
)

target_sources(
	fundament_opengl
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/private/gl_context.c
		${CMAKE_CURRENT_SOURCE_DIR}/public/fundament/gl_context.h
		${CMAKE_CURRENT_SOURCE_DIR}/private/gl_context_common.h
		${CMAKE_CURRENT_SOURCE_DIR}/private/gl_context_common.c
)

target_include_directories(
	fundament_opengl
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/public
)

target_link_libraries(
	fundament_opengl
	PUBLIC
		fundament::core
		fundament::window
)

add_library(
	fundament::opengl
	ALIAS
	fundament_opengl
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_sources(
		fundament_opengl
		PRIVATE
			${CMAKE_CURRENT_SOURCE_DIR}/private/win32/win32_gl_context.c
			${CMAKE_CURRENT_SOURCE_DIR}/private/win32/win32_gl_context.h
	)	

	target_link_libraries(
		fundament_opengl
		PUBLIC
			opengl32
			gdi32
	)
endif()
