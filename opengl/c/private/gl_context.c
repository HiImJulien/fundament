#include <fundament/gl_context.h>

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#if defined(_WIN32)
    #include "./win32/gl_context_win32.h"
	#include "gl/gl.h"
#else
    #include "./linux/gl_context_glx.h"
	#include "GL/gl.h"
#endif

static const uint32_t INDEX_MASK = 0x0FFF;
static const int fn__GL_NUM_EXTENSIONS = 0x821D;

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
        	(fn_native_window_handle_t) 0
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

bool fn_gl_context_extension_supported(const char* extension_name) {
	const char* version_info = glGetString(GL_VERSION);
	if(!version_info)
		return false;

	// As per specification:
	// > The GL_VERSION and GL_SHADING_LANGUAGE_VERSION strings begin with a 
	// > version number. The version number uses one of these forms:
	// >   major_number.minor_number major_number.minor_number.release_number
	// > Vendor-specific information may follow the version number. Its format 
	// > depends on the implementation, but a space always separates the version number and the vendor-specific information.
	// > All strings are null-terminated.
	const char major_str[] = {
		version_info[0],
		0
	};

	// glGetString(GL_EXTENSIONS) fails on OpenGL >= 3.0 using nVidia cards.
	// It is easier to use glGetStringi anyway, which is available on those versions.
	const int major = atoi(major_str);
	if(major >= 3) {
		typedef const GLubyte* (*fn__glGetStringi_t)(GLenum, GLuint);
		static fn__glGetStringi_t fn__glGetStringi;
		if(!fn__glGetStringi)
			fn__glGetStringi = (fn__glGetStringi_t) fn_gl_context_get_proc("glGetStringi");

		GLint extension_count = 0;
		glGetIntegerv(fn__GL_NUM_EXTENSIONS, &extension_count);

		for(int it = 0; it < extension_count; ++it) {
			const char* ext = (const char*) fn__glGetStringi(GL_EXTENSIONS, (GLuint) it);
			if(!ext)
				return false;

			if(strcmp(extension_name, ext) == 0)
				return true;
		}
	} else {
		const size_t ext_name_len = strlen(extension_name);
		const char* ext_list = (const char*) glGetString(GL_EXTENSIONS);
		const char* cursor = ext_list;
		const char* cursor_tail = ext_list;

		for(; *cursor != '\0'; cursor++)
			if(isspace(*cursor) != 0) {
				const size_t len = cursor - cursor_tail;
				
				if(len == ext_name_len
					&& memcmp(extension_name, cursor_tail, len) == 0)
					return true;

				cursor_tail = cursor + 1;
			}
	}


	return false;
}

fn_proc_t fn_gl_context_get_proc(const char* proc_name) {
	return fn__imp_gl_context_get_proc(proc_name);
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