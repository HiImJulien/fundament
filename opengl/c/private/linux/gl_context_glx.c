#include "./gl_context_glx.h"
#include <fundament/gl_context.h>
#include "../../../../platform/c/private/window_common.h"
// TODO: Fix this ugly shit.

fn__create_context_attribs fn__g_create_context_attribs = NULL;
fn__swap_interval fn__g_swap_interval = NULL;

void fn__imp_init_gl_module() {
    fn__g_create_context_attribs = (fn__create_context_attribs) 
        glXGetProcAddressARB("glXCreateContextAttribsARB");

    fn__g_swap_interval = (fn__swap_interval)
        glXGetProcAddressARB("glXSwapIntervalEXT");
}

void fn__imp_deinit_gl_module() {
}

bool fn__imp_create_gl_context(
    fn__opengl_context_t* ctx,
    const struct fn_gl_context_desc* desc) {

    const int visual_attribs[] = {
        GLX_X_RENDERABLE,   True,
        GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,    GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
        GLX_RED_SIZE,       desc->red_bits,
        GLX_GREEN_SIZE,     desc->green_bits,
        GLX_BLUE_SIZE,      desc->blue_bits,
        GLX_ALPHA_SIZE,     desc->alpha_bits,
        GLX_DEPTH_SIZE,     desc->depth_bits,
        GLX_STENCIL_SIZE,   desc->stencil_bits,
        GLX_DOUBLEBUFFER,   desc->is_double_buffered,
        None
    };

    int fbc_count = 0;
    GLXFBConfig* fbc = glXChooseFBConfig(
        fn__g_window_context.display,
        DefaultScreen(fn__g_window_context.display),
        visual_attribs,
        &fbc_count
    );
        
    int context_flags = 0;

    const int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, desc->major_version,
        GLX_CONTEXT_MINOR_VERSION_ARB, desc->minor_version,
        None
    };

    return false;
}

void fn__imp_destroy_gl_context(fn__opengl_context_t* ctx) {
}
