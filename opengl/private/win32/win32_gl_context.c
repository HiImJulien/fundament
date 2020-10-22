#include "win32_gl_context.h"
#include "../gl_context_common.h"
#include <fundament/gl_context.h>
#include <fundament/window.h>

#include <stdio.h>

typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC) (
    HDC hdc, 
    const int* piAttribIList, 
    const FLOAT* pfAttribFList, 
    UINT nMaxFormats,
    int* piFormats, 
    UINT* nNumFormats
);

typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (
    HDC hDC, 
    HGLRC hShareContext, 
    const int* attribList
);

typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);

static PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;
static PFNWGLCREATECONTEXTATTRIBSARBPROC   wglCreateContextAttribsARB;
static PFNWGLSWAPINTERVALEXTPROC           wglSwapintervalEXT;

const int WGL_DRAW_TO_WINDOW_ARB = 0x2001;
const int WGL_ACCELERATION_ARB = 0x2003;
const int WGL_SWAP_METHOD_ARB = 0x2007;
const int WGL_SUPPORT_OPENGL_ARB = 0x2010;
const int WGL_DOUBLE_BUFFER_ARB = 0x2011;
const int WGL_PIXEL_TYPE_ARB = 0x2013;
const int WGL_COLOR_BITS_ARB = 0x2014;
const int WGL_FULL_ACCELERATION_ARB = 0x2027;
const int WGL_SWAP_EXCHANGE_ARB = 0x2028;
const int WGL_TYPE_RGBA_ARB = 0x202B;
const int WGL_CONTEXT_MAJOR_VERSION_ARB = 0x2091;
const int WGL_CONTEXT_MINOR_VERSION_ARB = 0x2092;
const int WGL_RED_BITS_ARB = 0x2015;
const int WGL_GREEN_BITS_ARB = 0x2017;
const int WGL_BLUE_BITS_ARB = 0x2019;
const int WGL_ALPHA_BITS_ARB = 0x201B;
const int WGL_DEPTH_BITS_ARB = 0x2022;
const int WGL_STENCIL_BITS_ARB = 0x2023;

const int WGL_CONTEXT_FLAGS_ARB = 0x2094;
const int WGL_CONTEXT_DEBUG_BIT_ARB = 0x0001;
const int WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB = 0x0002;

const int WGL_CONTEXT_PROFILE_MASK_ARB = 0x9126;
const int WGL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001;
const int WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB = 0x00000002;


bool fn__init_win32_opengl() {
    if (wglChoosePixelFormatARB
        && wglCreateContextAttribsARB
        && wglSwapintervalEXT)
        return true;

    fn_init_window();
    struct fn_window dummy_window = fn_create_window();
    HWND dummy_hwnd = fn_window_handle(dummy_window);
    HDC dummy_dc = GetDC(dummy_hwnd);

    PIXELFORMATDESCRIPTOR pfd;
    HGLRC dummy_rc;
    SetPixelFormat(dummy_dc, 1, &pfd);
    
    dummy_rc = wglCreateContext(dummy_dc);
    wglMakeCurrent(dummy_dc, dummy_rc);

    wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)
        wglGetProcAddress("wglChoosePixelFormatARB");

    wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
        wglGetProcAddress("wglCreateContextAttribsARB");

    wglSwapintervalEXT = (PFNWGLSWAPINTERVALEXTPROC)
        wglGetProcAddress("wglSwapintervalEXT");

    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(dummy_rc);
    ReleaseDC(dummy_hwnd, dummy_dc);
    fn_destroy_window(dummy_window);

    return wglChoosePixelFormatARB 
        && wglCreateContextAttribsARB 
        && wglSwapintervalEXT;
}

void fn__deinit_win32_opengl() {}

bool fn__create_win32_gl_context(
    struct fn__win32_gl_context* context,
    const struct fn_gl_context_desc* desc
) {
    const int visual_attribs[] = {
        WGL_SUPPORT_OPENGL_ARB, TRUE,
        WGL_DRAW_TO_WINDOW_ARB, TRUE,
        WGL_DOUBLE_BUFFER_ARB,	desc->is_double_buffered,
        WGL_ACCELERATION_ARB,	WGL_FULL_ACCELERATION_ARB,
        WGL_COLOR_BITS_ARB,     desc->red_bits 
                                + desc->green_bits 
                                + desc->blue_bits,
        WGL_RED_BITS_ARB,		desc->red_bits,
        WGL_GREEN_BITS_ARB,		desc->green_bits,
        WGL_ALPHA_BITS_ARB,     desc->alpha_bits,
        WGL_BLUE_BITS_ARB,		desc->blue_bits,
        WGL_DEPTH_BITS_ARB,		desc->depth_bits,
        WGL_STENCIL_BITS_ARB,	desc->stencil_bits,
        WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,
        0
    };

    struct fn_window dummy_window = fn_create_window();
    HWND dummy_hwnd = fn_window_handle(dummy_window);
    HDC dummy_dc = GetDC(dummy_hwnd);

    int pixel_format;
    unsigned int pixel_format_count = 0;
    wglChoosePixelFormatARB(
        dummy_dc,
        visual_attribs,
        NULL,
        1,
        &pixel_format,
        &pixel_format_count
    );

    context->pixel_format = pixel_format;

    DescribePixelFormat(
        dummy_dc, 
        pixel_format, 
        sizeof(PIXELFORMATDESCRIPTOR), 
        &context->pfd
    );

    SetPixelFormat(dummy_dc, pixel_format, &context->pfd);

    int context_flags = 0;
    if (desc->is_debug)  context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB;

    int profile_mask = 0;
    if (desc->is_compatibility_profile)
        profile_mask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;
    else
        profile_mask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;

    const int context_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, desc->major_version,
        WGL_CONTEXT_MINOR_VERSION_ARB, desc->minor_version,
        WGL_CONTEXT_FLAGS_ARB, context_flags,
        WGL_CONTEXT_PROFILE_MASK_ARB, profile_mask,
        0
    };

    context->native = wglCreateContextAttribsARB(
        dummy_dc,
        0,
        context_attribs
    );

    ReleaseDC(dummy_hwnd, dummy_dc);
    fn_destroy_window(dummy_window);

    return context->native != NULL;
}

void fn__destroy_win32_gl_context(struct fn__win32_gl_context* context) {
    if (wglGetCurrentContext() == context->native)
        wglMakeCurrent(NULL, NULL);

    wglDeleteContext(context->native);
}

bool fn__make_win32_gl_context_current(
    struct fn__win32_gl_context* context,
    HWND hWnd
) {
    HDC hDC = GetDC(hWnd);
    if (context->pixel_format != GetPixelFormat(hDC))
        SetPixelFormat(hDC, context->pixel_format, &context->pfd);
    
    return wglMakeCurrent(hDC, context->native);
}

void fn__win32_gl_context_present() {
    HDC hDC = wglGetCurrentDC();
    if (hDC)
        SwapBuffers(hDC);
}
