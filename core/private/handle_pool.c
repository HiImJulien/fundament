#include <fundament/handle_pool.h>

#include <assert.h>
#include <stdlib.h>

bool fn_initialize_handle_pool(
    struct fn_handle_pool*  pool,
    size_t                  capacity
) {
    assert(pool && capacity);

    pool->unoccupied_ids        = malloc(sizeof(uint32_t) * capacity); 
    pool->unoccupied_ids_count  = capacity;
    pool->generations           = malloc(sizeof(uint8_t) * capacity);

    // Check, whether any allocation failed and release the other, if any.
    if(!pool->unoccupied_ids || !pool->generations) {
        free(pool->unoccupied_ids);
        free(pool->generations);
        return false;
    }

    size_t tmp = capacity;
    for(uint32_t it = 0; it < capacity; ++it)
        pool->unoccupied_ids[it] = (uint32_t) tmp--;
        
    for(uint8_t it = 0; it < capacity; ++it)
        pool->generations[it] = 0;

    return true;
} 

void fn_deinitialize_handle_pool(
    struct fn_handle_pool* pool
) {
    if(!pool)
        return;

    free(pool->unoccupied_ids);
    free(pool->generations);

    pool->unoccupied_ids        = NULL;
    pool->unoccupied_ids_count  = 0;
    pool->generations           = NULL;
}

bool fn_alloc_handle(
    struct fn_handle_pool*  pool,
    uint32_t*               handle,
    uint32_t*               index
) {
    assert(pool && handle && index);
    assert(pool->unoccupied_ids && pool->generations
            && "If this assertion is triggered, then the given pool is in a " \
               " bad state.");

    if(pool->unoccupied_ids_count == 0)
        return false;

    uint32_t tmp        = (uint32_t) --pool->unoccupied_ids_count;
    uint32_t tmp_handle = pool->unoccupied_ids[tmp];
    uint32_t tmp_gen    = pool->generations[tmp];

    *handle = (tmp_gen << 24) | tmp_handle;
    *index = tmp_handle - 1;

    return true;
}

void fn_dealloc_handle(
    struct fn_handle_pool*  pool,
    uint32_t                handle
) {
    if(!pool || !handle)
        return;

    const uint8_t generation    = (handle >> 24);
    const uint32_t id           = ~(0xFF << 24) & handle;

    if(id == 0)
        return;

    const uint32_t idx = id - 1;

    if(pool->generations[idx] != generation)
        return;

    pool->unoccupied_ids[pool->unoccupied_ids_count++] = id;
    ++pool->generations[idx];
}

bool fn_check_handle(
    struct fn_handle_pool*  pool,
    uint32_t                handle,
    uint32_t*               index
) {
    assert(pool);

    if(handle == 0)
        return false;

    const uint8_t generation    = (handle >> 24);
    const uint32_t id           = ~(0xFF << 24) & handle;

    if(id == 0)
        return false;

    const uint32_t idx = id - 1;

    if(pool->generations[idx] != generation)
        return false;

    if(index)
        *index = idx;

    return true;
}
