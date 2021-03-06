#ifndef FUNDAMENT_GL_CONTEXT_WIN32_H
#define FUNDAMENT_GL_CONTEXT_WIN32_H

#include <fundament/window.h>
#include <fundament/gl_context.h>

#include <stdbool.h>
#include <stdint.h>

#include <Windows.h>

struct fn_gl_context_desc;

//
// Encapsulates a win32 OpenGL context.
//
struct fn__opengl_context_win32 {
    uint8_t generation;
    HGLRC   handle;
    HDC     device_context;
    int 	pixel_format;
};

typedef struct fn__opengl_context_win32 fn__opengl_context_t;
typedef BOOL (WINAPI* fn__choose_pixel_format)(HDC, const int*, const FLOAT*, UINT, int*, UINT*);
typedef HGLRC (WINAPI* fn__create_context_attribs)(HDC, HGLRC, const int*);
typedef BOOL (WINAPI* fn__swap_interval)(int);

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
bool fn__imp_create_gl_context(
    fn__opengl_context_t* ctx,
    const struct fn_gl_context_desc* desc
);

//
// Destroys the OpenGL context.
//
void fn__imp_destroy_gl_context(fn__opengl_context_t* ctx);

//
// 
//
bool fn__imp_gl_context_make_current(
	fn__opengl_context_t* ctx,
	fn_native_window_handle_t win
);

//
//
//
void fn__imp_gl_context_present(void);

//
//
//
void fn__imp_gl_context_set_vsync(bool vsync);

//
//
//
fn_proc_t fn__imp_gl_context_get_proc(const char* proc_name);

#endif  // FUNDAMENT_GL_CONTEXT_WIN32_H
