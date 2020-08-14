#ifndef FUNDAMENT_GL_CONTEXT_H
#define FUNDAMENT_GL_CONTEXT_H

//==============================================================================
// This module provides a thin abstraction layer, allowing to create and 
// manage OpenGL contexts on linux and windows.
//==============================================================================

#include <fundament/api.h>
#include <fundament/window.h>

#include <stdbool.h>
#include <stdint.h>

//
// Initializes the OpenGL module.
//
API void fn_init_gl_module(void);

//
// Deinitializes the OpenGL module.
//
API void fn_deinit_gl_module(void);

//
// Represents a handle to an OpenGL context.
//
struct fn_gl_context{ uint32_t id; };

//
// Defines all creation parameters for an OpenGL context.
//
struct fn_gl_context_desc {
    uint8_t major_version;
    uint8_t minor_version;
    uint8_t red_bits;
    uint8_t green_bits;
    uint8_t blue_bits;
    uint8_t alpha_bits;
    uint8_t depth_bits;
    uint8_t stencil_bits;
    bool is_double_buffered;
    bool is_debug;
    bool is_compability_profile;
};

//
// Creates a new OpenGL context.
//
API struct fn_gl_context fn_create_gl_context(
    const struct fn_gl_context_desc* desc
);

//
// Destroys the OpenGL context.
//
API void fn_destroy_gl_context(
    struct fn_gl_context ctx
);

//
// Makes the given context current with the given window. 
//
API bool fn_gl_context_make_current(
    struct fn_gl_context ctx, 
    fn_native_window_handle_t win
);

//
// Presents the contents of the current contexts backbuffer.
//
API void fn_gl_context_present();

//
// Enables/Disables VSYNC.
//
// Note: If the extension EXT_swap_control_tear is present,
// adaptive VSYNC is being used. 
//
// This feature disables VSYNC, when the framerate drops below the refresh rate
// and enables it, when its above it.
// 
API void fn_gl_context_set_vsync(bool sync);

#if defined(_WIN32)
    typedef struct HGLRC__* fn_native_gl_context_handle_t;
#elif defined(__linux__)
    typedef struct __GLXcontext* fn_native_gl_context_handle_t;
#else
    #error "gl_context is only supported on windows (Win32) and linux (GLX)."
#endif

//
// Returns the native handle of the OpenGL context.
//
// Returns NULL, if the passed context is deemed invalid.
//
API fn_native_gl_context_handle_t fn_gl_context_handle(
    struct fn_gl_context ctx
);



#endif  // FUNDAMENT_GL_CONTEXT_H
