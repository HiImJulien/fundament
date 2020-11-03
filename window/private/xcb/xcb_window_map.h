#ifndef FUNDAMENT_XCB_WINDOW_MAP_H
#define FUNDAMENT_XCB_WINDOW_MAP_H

//==============================================================================
//                               XCB WINDOW MAP
//
// Implements a hash map, that maps from xcb's window ids to pointers of
// `fn__window`.
//
//==============================================================================

#include <stdint.h>

struct fn__window;

void fn__xcb_window_map_insert(struct fn__window* window);

void fn__xcb_window_map_remove(struct fn__window* window);

struct fn__window* fn__xcb_window_map_get(uint32_t id);


#endif  // FUNDAMENT_XCB_WINDOW_MAP_H

