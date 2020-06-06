#ifndef FUNDAMENT_INPUT_COMMON_H
#define FUNDAMENT_INPUT_COMMON_H

//==============================================================================
// This file provides a common interface to the interal key state cache.
//==============================================================================

#include <fundament/input.h>

#include <stdbool.h>

//
// Sets the state for the given key in cache.
//
void fn__set_key_state(enum fn_key key, bool pressed);

//
// Reads the state for the given key from cache.
//
bool fn__get_key_state(enum fn_key key);

//
// Sets the state for a given button in cache.
//
// Note, that this function only works single button values.
//
void fn__set_button_state(enum fn_button button, bool pressed);

//
// Reads the state for the given buttons from cache.
//
bool fn__get_button_state(enum fn_button buttons);

//
// Returns the currently pressed mouse buttons.
//
enum fn_button fn__get_pressed_buttons();

#endif  // FUNDAMENT_INPUT_COMMON_H