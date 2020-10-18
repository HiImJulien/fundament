#ifndef GL_CONTEXT_COMMON_H
#define GL_CONTEXT_COMMON_H

//==============================================================================
//                               OPENGL COMMON
//
// Provides functions and data structures used by both, the public API and the
// internal implementation.
//
//==============================================================================

#include <fundament/handle_pool.h>
#include <fundament/types.h>

#if defined(_WIN32)
	#include "win32/win32_gl_context.h"
#endif

#define FN_GL_CONTEXT_CAPACITY	8

struct fn__gl_mod_context {
	struct fn__gl_context	contexts[FN_GL_CONTEXT_CAPACITY];
	struct fn_handle_pool	context_pool;
};

extern struct fn__gl_mod_context fn__g_gl_context;

#endif	// GL_CONTEXT_COMMON_H
