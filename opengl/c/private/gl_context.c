#include <fundament/gl_context.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(_WIN32)
    #include "./win32/gl_context_win32.h"
#else
    #include "./linux/gl_context_glx.h"
#endif

//
// Encapsulates global variables used by this module.
//
struct fn__gl_mod_context {
    fn__opengl_context_t pool[8];
};

static struct fn__gl_mod_context* fn__g_gl_mod_context = NULL;

void fn_init_gl_module() {
    if(fn__g_gl_mod_context != NULL)
        // TODO: Warn, that you're trying to init without releasing previous instance.
        return;

    fn__g_gl_mod_context = malloc(sizeof(struct fn__gl_mod_context));
    memset(fn__g_gl_mod_context, 0, sizeof(struct fn__gl_mod_context));

    fn__imp_init_gl_module();
}

void fn_deinit_gl_module() {
    fn__imp_deinit_gl_module();

    free(fn__g_gl_mod_context);
    fn__g_gl_mod_context = NULL;
}

struct fn_gl_context fn_create_gl_context(
    const struct fn_gl_context_desc* desc
) {
    uint32_t idx = 0;
    for(; idx <= 8; ++idx) {
        if(idx >= 8)
            return (struct fn_gl_context) { 0 };

        if(!fn__g_gl_mod_context->pool[idx].handle)
            break;
    }

    const bool success = fn__imp_create_gl_context(
        &fn__g_gl_mod_context->pool[idx],
        desc
    );

    return (struct fn_gl_context) { success ? idx + 1 : 0 };
}

void fn_destroy_gl_context(struct fn_gl_context ctx) {

}

bool fn_gl_context_make_current(
    struct fn_gl_context ctx,
    fn_native_window_handle_t win
) {
    if(ctx.id == 0)
        return false;

    return fn__imp_gl_context_make_current(
        &fn__g_gl_mod_context->pool[ctx.id - 1],
        win
    );
}

void fn_gl_context_present() {
    fn__imp_gl_context_present();
}