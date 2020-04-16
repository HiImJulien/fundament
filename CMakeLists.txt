project(fundament)
cmake_minimum_required(VERSION 3.9)

add_library(
	fundament
	"${CMAKE_CURRENT_SOURCE_DIR}/lib/c/private/fundament.c"
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
	)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	enable_language(OBJC)
	set_source_files_properties(
		"${CMAKE_CURRENT_SOURCE_DIR}/lib/c/private/fundament.c"
		PROPERTIES
		LANGUAGE OBJC
	)

	target_link_libraries(
		fundament
		PUBLIC
		"-framework AppKit"
		"-framework IOKit"
		"-framework CoreFoundation"
	)
endif()

add_executable(
	fundament_simple_window
	"${CMAKE_CURRENT_SOURCE_DIR}/examples/c/simple_window.c"	
)

target_link_libraries(
	fundament_simple_window
	PRIVATE
	fundament
)