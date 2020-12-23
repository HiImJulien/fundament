#ifndef FUNDAMENT_HANDLE_CONTAINER_H
#define FUNDAMENT_HANDLE_CONTAINER_H

//==============================================================================
//                              HANDLE CONTAINER
//
// Fundament makes heavy use of handles to reference resources.
// A handle is a typed 32-bit unsigned integer having a "generation" and "id"
// encoded.
//
//                                  HANDLES
//                    |--------|------------------------|
//                    |   gen  |            id          |
//                    |--------|------------------------|
//                    | 8 bit  |          32 bit        |
//                    |--------|------------------------|
//
// Generations are used to detect dangling accesses:
// - Each slot has a generations counter, which is incremented, whenever the 
//   object stored in the slot is being destroyed.
// - When accessing the object, the handle's generation is compared to the 
//   corresponding slot's generation. If the generations do not match, it's a
//   dangling access.
//
// IDs are just incremented indexes into an array storing the referenced object.
//
//==============================================================================

#include <fundament/api.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct fn_handle_container {
    uint32_t*   unoccupied_ids;
    uint32_t    unoccupied_ids_count;
    uint8_t*    generations;
};

API bool fn_initialize_handle_container(
    struct fn_handle_container* container,
    size_t                      capacity
);

API void fn_deinitialize_handle_container(
    struct fn_handle_container* container
);

//
// Attempts to allocate (i.e. reserve) a new handle,
// writes it to `handle`, the corresponding index to `index`
// and returns true.
//
// Returns false if the container is exhausted, and leaves 
// `handle` and `index` unchanged.
//
API bool fn_alloc_handle(
    struct fn_handle_container* container,
    uint32_t*                   handle,
    uint32_t*                   index
);

//
// Attempts to deallocate (i.e. free) the handle and returns true.
//
// Returns false if the handle is not valid.
//
API bool fn_dealloc_handle(
    struct fn_handle_container* container,
    uint32_t                    handle
);

//
// Checks whether the given handle is valid or not.
// Optionally retrieves the corresponding index.
//
API bool fn_check_handle(
    struct fn_handle_container* container,
    uint32_t                    handle,
    uint32_t*                   index
);

#endif  // FUNDAMENT_HANDLE_CONTAINER_H

