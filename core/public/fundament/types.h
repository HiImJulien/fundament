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
    
#endif  // FUNDAMENT_TYPES_H

