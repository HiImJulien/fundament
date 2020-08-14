#include "./gl_context_win32.h"
#include <fundament/gl_context.h>

fn__choose_pixel_format 	fn__g_choose_pixel_format = NULL;
fn__create_context_attribs 	fn__g_create_context_attribs = NULL;
fn__swap_interval 			fn__g_swap_interval = NULL;

const int WGL_DRAW_TO_WINDOW_ARB         = 0x2001;
const int WGL_ACCELERATION_ARB           = 0x2003;
const int WGL_SWAP_METHOD_ARB            = 0x2007;
const int WGL_SUPPORT_OPENGL_ARB         = 0x2010;
const int WGL_DOUBLE_BUFFER_ARB          = 0x2011;
const int WGL_PIXEL_TYPE_ARB             = 0x2013;
const int WGL_COLOR_BITS_ARB             = 0x2014;
const int WGL_FULL_ACCELERATION_ARB      = 0x2027;
const int WGL_SWAP_EXCHANGE_ARB          = 0x2028;
const int WGL_TYPE_RGBA_ARB              = 0x202B;
const int WGL_CONTEXT_MAJOR_VERSION_ARB  = 0x2091;
const int WGL_CONTEXT_MINOR_VERSION_ARB  = 0x2092;
const int WGL_RED_BITS_ARB               = 0x2015;
const int WGL_GREEN_BITS_ARB             = 0x2017;
const int WGL_BLUE_BITS_ARB              = 0x2019;
const int WGL_ALPHA_BITS_ARB             = 0x201B;
const int WGL_DEPTH_BITS_ARB             = 0x2022;
const int WGL_STENCIL_BITS_ARB           = 0x2023;

const int WGL_CONTEXT_FLAGS_ARB 		 			= 0x2094;
const int WGL_CONTEXT_DEBUG_BIT_ARB               	= 0x0001;
const int WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  	= 0x0002;

const int WGL_CONTEXT_PROFILE_MASK_ARB	 			= 0x9126;
const int WGL_CONTEXT_CORE_PROFILE_BIT_ARB        	= 0x00000001;
const int WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB = 0x00000002;

void fn__imp_init_gl_module() {
	struct fn_window dummy_window = fn_create_window();

	HWND dummy_handle = fn_window_handle(dummy_window);
	HDC dummy_device = GetDC(dummy_handle);

	PIXELFORMATDESCRIPTOR pfd;
	SetPixelFormat(dummy_device, 1, &pfd);

	HGLRC dummy_context = wglCreateContext(dummy_device);
	wglMakeCurrent(dummy_device, dummy_context);

	fn__g_choose_pixel_format = (fn__choose_pixel_format)
		wglGetProcAddress("wglChoosePixelFormatARB");

	fn__g_create_context_attribs = (fn__create_context_attribs)
		wglGetProcAddress("wglCreateContextAttribsARB");

	fn__g_swap_interval = (fn__swap_interval)
		wglGetProcAddress("wglSwapIntervalEXT");

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(dummy_context);
	ReleaseDC(dummy_handle, dummy_device);

	fn_destroy_window(dummy_window);
}

void fn__imp_deinit_gl_module() {}

bool fn__imp_create_gl_context(
    fn__opengl_context_t* ctx,
    const struct fn_gl_context_desc* desc) {

	const int visual_attribs[] = {
		WGL_SUPPORT_OPENGL_ARB, TRUE,
		WGL_DRAW_TO_WINDOW_ARB, TRUE,
		WGL_DOUBLE_BUFFER_ARB,	desc->is_double_buffered,
		WGL_ACCELERATION_ARB,	WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,
		desc->red_bits + desc->green_bits + desc->blue_bits,
		WGL_RED_BITS_ARB,		desc->red_bits,
		WGL_GREEN_BITS_ARB,		desc->green_bits,
		WGL_BLUE_BITS_ARB,		desc->blue_bits,
		WGL_DEPTH_BITS_ARB,		desc->depth_bits,
		WGL_STENCIL_BITS_ARB,	desc->stencil_bits,
		WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,
		0
	};

	struct fn_window dummy_window = fn_create_window();
	HWND dummy_handle = fn_window_handle(dummy_window);
	HDC dummy_device = GetDC(dummy_handle);

	int pixel_format;
	unsigned int pixel_format_count = 0;
	fn__g_choose_pixel_format(
		dummy_device, 
		visual_attribs, 
		NULL, 
		1, 
		&pixel_format, 
		&pixel_format_count
	);

	PIXELFORMATDESCRIPTOR pfd;
	SetPixelFormat(dummy_device, pixel_format, &pfd);

	int context_flags = 0;
	if(desc->is_debug)
		context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;


	int profile_mask = 0;
	if(desc->is_compability_profile)
		profile_mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
	else
		profile_mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

	const int context_attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, desc->major_version,
		WGL_CONTEXT_MINOR_VERSION_ARB, desc->major_version,
		WGL_CONTEXT_FLAGS_ARB, context_flags,
		WGL_CONTEXT_PROFILE_MASK_ARB, profile_mask,
		0
	};

	ctx->handle = fn__g_create_context_attribs(dummy_device, 0, context_attribs);

	ReleaseDC(dummy_handle, dummy_device);
	fn_destroy_window(dummy_window);

	ctx->pixel_format = pixel_format;
    return ctx->handle != NULL;
}

void fn__imp_destroy_gl_context(fn__opengl_context_t* ctx) {
	wglDeleteContext(ctx->handle);
}

bool fn__imp_gl_context_make_current(
	fn__opengl_context_t* ctx,
	fn_native_window_handle_t win
) {
	if(ctx == NULL)
		return wglMakeCurrent(NULL, NULL);

	HDC dc = GetDC(win);
	int pixel_format = GetPixelFormat(dc);
	if(pixel_format > 0 && pixel_format != ctx->pixel_format)
		return false;

	if(pixel_format == 0) {
		PIXELFORMATDESCRIPTOR pfd;
		SetPixelFormat(dc, ctx->pixel_format, &pfd);
	}

	return wglMakeCurrent(GetDC(win), ctx->handle);
}

void fn__imp_gl_context_present() {
	HDC current_dc = wglGetCurrentDC();
	if(current_dc)
		SwapBuffers(current_dc);
}

void fn__imp_gl_context_set_vsync(bool vsync) {
    if(fn__g_swap_interval == NULL)
        return;

    fn__g_swap_interval(vsync ? 1 : 0);
}