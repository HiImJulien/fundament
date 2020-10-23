#ifndef FUNDAMENT_TYPES_H
#define FUNDAMENT_TYPES_H

//==============================================================================
//                                   TYPES
//
// This header declares types or type aliases for types, that used by multiple
// modules.
//==============================================================================

#include <stdint.h>

#if defined(__APPLE__) && defined(__OBJC__)
    @class NSWindow;
    typedef NSWindow* fn_native_window_handle_t;
#elif defined(__APPLE__) 
    typedef void* fn_native_window_handle_t;
#elif defined(_WIN32)
    typedef struct HWND__* fn_native_window_handle_t;
#elif defined(__linux__)
    typedef uint32_t fn_native_window_handle_t;
#else
    #error "Unknown target!"
#endif
    
#if defined(__APPLE__)
    // Apple deprecated OpenGL.
    // I don't see a use in supporting Apple.
    // - Kirsch
#elif defined(_WIN32)
    typedef struct HGLRC__* fn_native_gl_handle_t;
#elif defined(__linux__)
    typedef struct __GLXcontextRec* fn_native_gl_handle_t;
#endif

#if defined(__STDC_VERSION__ ) && __STDC_VERSION__ >= 201112L
    #define FN_THREAD_LOCAL _Thread_local
#elif defined(_MSC_VER)
    #define FN_THREAD_LOCAL __declspec(thread)
#elif defined(__GNUC__) || defined(__clang__)
    #define FN_THREAD_LOCAL __thread;
#else
    #define FN_THREAD_LOCAL
#endif

#define FN_WINDOW_CAPACITY              64
#define FN_INITIAL_EVENT_CAPACITY       256

#endif  // FUNDAMENT_TYPES_H

