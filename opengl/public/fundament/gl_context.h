#ifndef FUNDAMENT_GL_CONTEXT_H
#define FUNDAMENT_GL_CONTEXT_H

//==============================================================================
//                               OPENGL CONTEXT
//
// OpenGL context, representing the state of the pipeline.
//
// This module provides functions to create and maintain an OpenGL context.
//
//==============================================================================

#include <fundament/types.h>

#include <stdbool.h>
#include <stdint.h>

struct fn_gl_context { uint32_t id; };

//
// Initializes the OpenGL module.
//
bool fn_init_opengl();

//
// Deinitializes the OpenGL module.
//
void fn_deinit_opengl();

//
// Describes an OpenGL context.
//
struct fn_gl_context_desc {
	uint8_t	major_version;
	uint8_t	minor_version;

	uint8_t	red_bits;
	uint8_t	green_bits;
	uint8_t	blue_bits;
	uint8_t	alpha_bits;
	uint8_t	depth_bits;
	uint8_t	stencil_bits;

	bool	is_double_buffered;
	bool	is_debug;
	bool	is_compaitbility_profile;
};

//
// Creates a new OpenGL context.
//
struct fn_gl_context fn_create_gl_context(
	const struct fn_gl_context_desc* desc
);

//
// Destroys the OpenGL context.
//
void fn_destroy_gl_context(struct fn_gl_context context);

//
// Makes the specified context current.
//
bool fn_gl_context_make_current(
	struct fn_gl_context context,
	fn_native_window_handle_t window
);

//
// Presents the current contents of the current context's backbuffer.
//
void fn_gl_context_present();

#endif	// FUNDAMENT_GL_CONTEXT_H
