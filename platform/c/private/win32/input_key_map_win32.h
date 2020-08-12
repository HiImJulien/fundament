#ifndef FUNDAMENT_INPUT_KEY_MAP_WIN32_H
#define FUNDAMENT_INPUT_KEY_MAP_WIN32_H

#include <stdint.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//==============================================================================
// This file was generated using the keymap_gen.py script, which is part of the
// fundament project and subject to the project's license.
//==============================================================================

//
// Maps win32's native keycodes to fundament's keycodes.
// Return's 'fn_key_undefined', if the underlying platform does not
// provide a mapping for given key.
//
uint16_t fn__imp_map_win32_key(WPARAM key_code);

#endif  // FUNDAMENT_INPUT_KEY_MAP_WIN32_H

