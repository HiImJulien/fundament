#ifndef FUNDAMENT_GL_CONTEXT_WIN32_H
#define FUNDAMENT_GL_CONTEXT_WIN32_H

#include <stdbool.h>
#include <stdint.h>

#include <Windows.h>

//
// Encapsulates a win32 OpenGL context.
//
struct fn__opengl_context_win32 {
	uint8_t	generation;
	HGLRC 	handle;
	HDC 	device_context;
};

typedef struct fn__opengl_context_win32 fn__opengl_context_t;
typedef BOOL (WINAPI* fn__choose_pixel_format)(HDC, const int, const FLOAT, UINT, int, UINT);
typedef BOOL (WINAPI* fn__create_context_attribs)(HDC, HGLRC, const int*);
typedef BOOL (WINAPI* fn__swap_interval)(int);

extern fn__choose_pixel_format fn__g_choose_pixel_format;
extern fn__create_context_attribs fn__g_create_context_attribs;
extern fn__swap_interval fn__g_swap_interval;

//
// Creates a throw-away Window and OpenGL context, which is required,
// to retrieve the ARB WGL extensions, which are needed to create any 
// OpenGL context for versions above 1.1.
//
// Calling this function causes gdi32.dll and opengl32.dll to be loaded
// lazily.
// 
void fn__imp_init_gl_module(void);

//
// Does nothing on Win32.
//
void fn__imp_deinit_gl_module(void);

//
// Creates a new OpenGL context.
//
bool fn__imp_create_gl_context(fn__opengl_context_t* ctx);

//
// Destroys the OpenGL context.
//
void fn__imp_destroy_gl_context(fn__opengl_context_t* ctx);

#endif 	// FUNDAMENT_GL_CONTEXT_WIN32_H
