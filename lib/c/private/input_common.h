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

#endif  // FUNDAMENT_INPUT_COMMON_H