#include <fundament/gl_context.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(_WIN32)
    #include "./win32/gl_context_win32.h"
#else
    #include "./linux/gl_context_glx.h"
#endif

static const uint32_t INDEX_MASK = 0x0FFF;

//
// Retrieves the index from an id.
// 
static inline uint32_t fn__idx(uint32_t id) {
	return (id & INDEX_MASK) - 1;
}

//
// Retrieves the generation from an id.
//
static inline uint8_t fn__gen(uint32_t id) {
	return id >> 24;
}

//
// Makes an id from an index and generation.
//
static inline uint32_t fn__id(uint32_t idx, uint8_t generation) {
	uint8_t* nibbles = (uint8_t*) &idx;
	nibbles[3] = generation;
	return idx + 1;
}

//
// Encapsulates global variables used by this module.
//
struct fn__gl_mod_context {
    fn__opengl_context_t pool[8];
    uint8_t generation[8];
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

    for(uint8_t it = 0; it < 8; ++it)
    	if(fn__g_gl_mod_context->pool[it].handle != NULL) {
    		fn__imp_destroy_gl_context(
    			&fn__g_gl_mod_context->pool[it]
			);
		}

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

    const uint8_t gen = fn__g_gl_mod_context->generation[idx];
    return (struct fn_gl_context) { success ? fn__id(idx, gen) : 0 };
}

void fn_destroy_gl_context(struct fn_gl_context ctx) {
	if(ctx.id == 0)
		return;

    const uint8_t gen = fn__gen(ctx.id);
    const uint32_t idx = fn__idx(ctx.id);

    if(fn__g_gl_mod_context->generation[idx] != gen)
    	return;

    fn__imp_destroy_gl_context(&fn__g_gl_mod_context->pool[idx]);
}

bool fn_gl_context_make_current(
    struct fn_gl_context ctx,
    fn_native_window_handle_t win
) {
    if(ctx.id == 0)
        return fn__imp_gl_context_make_current(
        	NULL,
        	NULL
    	);

    const uint8_t gen = fn__gen(ctx.id);
    const uint32_t idx = fn__idx(ctx.id);

    if(fn__g_gl_mod_context->generation[idx] != gen)
    	return false;

    return fn__imp_gl_context_make_current(
        &fn__g_gl_mod_context->pool[idx],
        win
    );

    fn__g_gl_mod_context->generation[idx]++;
}

void fn_gl_context_present() {
    fn__imp_gl_context_present();
}

void fn_gl_context_set_vsync(bool vsync) {
    fn__imp_gl_context_set_vsync(vsync);
}

fn_native_gl_context_handle_t fn_gl_context_handle(struct fn_gl_context ctx) {
	if(ctx.id == 0)
		return false;

    const uint8_t gen = fn__gen(ctx.id);
    const uint32_t idx = fn__idx(ctx.id);

    if(fn__g_gl_mod_context->generation[idx] != gen)
    	return false;

	return fn__g_gl_mod_context->pool[fn__idx(ctx.id)].handle;
}