#include <fundament/gl_context.h>
#include "gl_context_common.h"

#if defined(_WIN32)
	#include "win32/win32_gl_context.h"
#endif

bool fn_init_opengl() {
	fn_initialize_handle_pool(
		&fn__g_gl_context.context_pool,
		FN_GL_CONTEXT_CAPACITY
	);

	return fn__init_imp_opengl();
}

void fn_deinit_opengl() {
	fn_deinitialize_handle_pool(
		&fn__g_gl_context.context_pool
	);

	fn__deinit_imp_opengl();
}

struct fn_gl_context fn_create_gl_context(
	const struct fn_gl_context_desc* desc
) {
	uint32_t handle, index;
	if (!fn_alloc_handle(&fn__g_gl_context.context_pool, &handle, &index))
		return (struct fn_gl_context) { 0 };

	struct fn__gl_context* ctx = &fn__g_gl_context.contexts[index];
	if (!fn__create_imp_gl_context(ctx, desc)) {
		fn_dealloc_handle(&fn__g_gl_context.context_pool, handle);
		return (struct fn_gl_context) { 0 };
	}

	return (struct fn_gl_context) { handle };
}

void fn_destroy_gl_context(struct fn_gl_context context) {
	uint32_t index;
	if (!fn_check_handle(&fn__g_gl_context.context_pool, context.id, &index))
		return;

	struct fn__gl_context* ptr = &fn__g_gl_context.contexts[index];
	fn__destroy_imp_gl_context(ptr);
	fn_dealloc_handle(&fn__g_gl_context.context_pool, context.id);
}

bool fn_gl_context_make_current(
	struct fn_gl_context context,
	fn_native_window_handle_t window
) {
	uint32_t index;
	if (!fn_check_handle(&fn__g_gl_context.context_pool, context.id, &index))
		return false;

	struct fn__gl_context* ptr = &fn__g_gl_context.contexts[index];
	return fn__imp_gl_context_make_current(ptr, window);
}

void fn_gl_context_present() {
	fn__imp_gl_context_present();
}