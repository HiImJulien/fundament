#ifndef FUNDAMENT_WIN32_GL_CONTEXT_H
#define FUNDAMENT_WIN32_GL_CONTEXT_H

//==============================================================================
//                            WIN32 OPENGL CONTEXT
//
// Implements OpenGL contexts using the WGL api.
//
//==============================================================================

#include <stdbool.h>

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct fn__win32_gl_context {
	HGLRC					native;
	HDC						device;
	int						pixel_format;
	PIXELFORMATDESCRIPTOR	pfd;
};

struct fn_gl_context_desc;

bool fn__init_win32_opengl();
void fn__deinit_win32_opengl();

bool fn__create_win32_gl_context(
	struct fn__win32_gl_context* context,
	const struct fn_gl_context_desc* desc
);

void fn__destroy_win32_gl_context(struct fn__win32_gl_context* context);

bool fn__make_win32_gl_context_current(
	struct fn__win32_gl_context* context,
	HWND hWnd
);

void fn__win32_gl_context_present();

#define fn__gl_context					fn__win32_gl_context
#define fn__init_imp_opengl				fn__init_win32_opengl
#define fn__deinit_imp_opengl			fn__deinit_win32_opengl
#define fn__create_imp_gl_context		fn__create_win32_gl_context
#define fn__destroy_imp_gl_context		fn__destroy_win32_gl_context
#define fn__imp_gl_context_make_current	fn__make_win32_gl_context_current
#define fn__imp_gl_context_present		fn__win32_gl_context_present
#define fn__imp_gl_context_get_proc     wglGetProcAddress

#endif	// FUNDAMENT_WIN32_GL_CONTEXT_H
