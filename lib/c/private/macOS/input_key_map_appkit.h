#ifndef FUNDAMENT_INPUT_KEY_MAP_APPKIT_H
#define FUNDAMENT_INPUT_KEY_MAP_APPKIT_H

#include <stdint.h>



//==============================================================================
// This file was generated using the keymap_gen.py script, which is part of the
// fundament project and subject to the project's license.
//==============================================================================

//
// Maps appkit's native keycodes to fundament's keycodes.
// Return's 'fn_key_undefined', if the underlying platform does not
// provide a mapping for given key.
//
uint16_t fn__imp_map_appkit_key(uint16_t key_code);

#endif  // FUNDAMENT_INPUT_KEY_MAP_APPKIT_H

