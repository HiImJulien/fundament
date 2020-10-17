#ifndef FUNDAMENT_HANDLE_POOL_H
#define FUNDAMENT_HANDLE_POOL_H

//==============================================================================
//                                HANDLE POOL
//
// An utility for managing handles.
//
// Fundament makes heavy use of handles, which are managed by handle pools.
// A handle is an unsigned 32-bit integer, wrapped inside a structure to ensure
// type safety, which encodes a "generation" and an "id" as seen in the
// following table:
//
//                    |--------|------------------------|
//                    | 8 bit  |         24 bit         |
//                    |--------|------------------------|
//                    |   gen  |          id            |
//                    |--------|------------------------|
//
// Generations are used to detect and prevent danling accesses, where
// "old" IDs are errournously used to reference objects that were already
// freed.
//
//==============================================================================

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

//
// Represents a pool used to manage handles and their generations.
//
struct fn_handle_pool {
    uint32_t*   unoccupied_ids;
    size_t      unoccupied_ids_count;
    uint8_t*    generations;
};

//
// Initializes the handle pool to support up to `capacity` handles, and returns
// true.
// Returns false, if the initialization fails.
//
bool fn_initialize_handle_pool(
    struct fn_handle_pool*  pool,
    size_t                  capacity
); 

//
// Deinitializes the pool, releasing resources allocated by it.
//
void fn_deinitialize_handle_pool(
    struct fn_handle_pool*  pool
);

//
// Tries to allocate a new handle, writing the result to `handle` and its 
// corresponding index to `index`.
//
// Returns false, if the allocation fails due exhaustion of the pool.
//
bool fn_alloc_handle(
    struct fn_handle_pool*  pool,
    uint32_t*               handle,
    uint32_t*               index
);

//
// Deallocates a handle; i.e. release it.
//
void fn_dealloc_handle(
    struct fn_handle_pool*  pool,
    uint32_t                handle
);

//
// Returns true if the handle is valid and writes the corresponding index
// to `index`, if `index` is not NULL.
// Returns false otherwise.
//
bool fn_check_handle(
    struct fn_handle_pool*  pool,
    uint32_t                handle,
    uint32_t*               index
);

#endif  // FUNDAMENT_HANDLE_POOL_H

