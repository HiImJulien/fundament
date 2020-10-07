#ifndef FUNDAMENT_ID_H
#define FUNDAMENT_ID_H

//==============================================================================
// This module provides utilities for working with IDs.
//==============================================================================

#include <stdint.h>

//
// Declares a specific handle.
//
#define FN_HANDLE(x) struct fn_##x { uint32_t id; } 

//
// Utility, helps to check whether a handle is valid.
//
// Note:
//   - This utility does NOT check, whether the handle expired.
//
#define FN_HANDLE_VALID(x) (x.id != 0)

//==============================================================================
// The following macros, functions, structs, etc. are used mostly internally.
//==============================================================================

//
// Utility, used INTERNALLY to return invalid handles.
//
// Usually indicates, that an error has occured while trying to allocate
// a specific resource.
//
#define FN_INVALID(x) (struct x) { .id = 0 }

//
// Utility, used INTERNALLY to retrieve the encoded generation from an id.
//
// Note:
// - Assumes, that the upper 8 bit contain the generation.
//
inline static uint8_t fn_handle_get_generation(uint32_t id) {
    const uint8_t* nibbles = &id; 
    return nibbles[3];
}

//
// Utility, used INTERNALLY to retrieve the encoded index from an id.
//
// Note:
// - Assumes, that the lower three bytes contain the index + 1.
//
inline static uint32_t fn_handle_get_index(uint32_t id) {
    return id == 0 ? 0 : (id & 0xFFFFFF) - 1;
}

#endif  // FUNDAMENT_ID_H
