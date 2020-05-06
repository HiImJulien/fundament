project(fundament)
cmake_minimum_required(VERSION 3.9)

set(BUILD_EXAMPLES FALSE CACHE BOOL "Whether to build the examples.")
set(BUILD_PLAYGROUND FALSE CACHE BOOL "Whether to build the playground.")

add_library(
	fundament
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/c/private/fundament.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/c/private/gfx.c"
)

target_include_directories(
	fundament
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/c/public"
)

set_target_properties(
	fundament
	PROPERTIES
	C_STANDARD 11
)

if(WIN32)
	target_compile_definitions(
		fundament
		PRIVATE
		_CRT_SECURE_NO_WARNINGS
	)

	target_link_libraries(
		fundament
		PUBLIC
		user32.lib
		d3d11.lib
		dxguid.lib
		d3dcompiler.lib
	)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	enable_language(OBJC)
	set_source_files_properties(
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/c/private/fundament.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/c/private/gfx.c"
		PROPERTIES
		LANGUAGE OBJC
	)

	target_link_libraries(
		fundament
		PUBLIC
		"-framework AppKit"
		"-framework IOKit"
		"-framework CoreFoundation"
		"-framework Metal"
		"-framework QuartzCore"
	)
endif()

if(BUILD_EXAMPLES)
	add_executable(
		fundament_simple_window
		"${CMAKE_CURRENT_SOURCE_DIR}/examples/c/simple_window.c"	
	)

	target_link_libraries(
		fundament_simple_window
		PRIVATE
		fundament
	)

	add_executable(
		fundament_simple_triangle
		"${CMAKE_CURRENT_SOURCE_DIR}/examples/c/simple_triangle.c"
	)

	target_link_libraries(
		fundament_simple_triangle
		PRIVATE
		fundament
	)
endif()

if(BUILD_PLAYGROUND)
	message(INFO "Building playground.")
	add_subdirectory(playground)
endif()
