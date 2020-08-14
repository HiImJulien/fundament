#ifndef FUNDAMENT_GL_CONTEXT_GLX_H
#define FUNDAMENT_GL_CONTEXT_GLX_H

#include <fundament/window.h>

#include <stdbool.h>
#include <stdint.h>

#include <X11/Xlib.h>
#include <GL/glx.h>

struct fn_gl_context_desc;

//
// Encapsulates a glx OpenGL context.
//
struct fn__opengl_context_glx {
    uint8_t     generation;
    GLXContext  handle;
};

typedef struct fn__opengl_context_glx fn__opengl_context_t;
typedef GLXContext (*fn__create_context_attribs)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
typedef void (*fn__swap_interval)(Display*, GLXDrawable, int interval);

//
// Retrieves the ARB GLX extensions.
//
void fn__imp_init_gl_module(void);

//
// Does nothing on linux/glx.
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
// Sets the context current to the thread and window.
//
bool fn__imp_gl_context_make_current(
    fn__opengl_context_t* ctx,
    fn_native_window_handle_t win
);

//
// Displays the contents of the current drawable's backbuffer.
//
void fn__imp_gl_context_present(void);

//
// Enables/Disables vsync on the current context.
//
void fn__imp_gl_context_set_vsync(bool vsync);

#endif  // FUNDAMENT_GL_CONTEXT_GLX_H

