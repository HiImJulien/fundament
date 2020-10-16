#include <fundament/handle_container.h>

#include <assert.h>
#include <stdlib.h>

bool fn_initialize_handle_container(
    struct fn_handle_container* container,
    size_t                      capacity
) {
    assert(container && capacity);

    uint32_t* base = malloc(
        sizeof(uint32_t) * capacity
        + sizeof(uint8_t) * capacity
    );

    if(!base)
        return false;

    container->unoccupied_ids = base;
    container->unoccupied_ids_count = capacity;
    container->generations = (uint8_t*) &base[capacity];

    uint32_t tmp = (uint32_t) capacity;
    for(uint32_t it = 0; it < capacity; ++it)
        container->unoccupied_ids[it] = tmp--;
    
    for(uint8_t it = 0; it < capacity; ++it)
        container->generations[it] = 0;

    return true;
}

void fn_deinitialize_handle_container(
    struct fn_handle_container* container
) {
    if(!container)
        return;

    // Only freeing unoccupied_ids is required;
    // since the generations are allocated with them.
    free(container->unoccupied_ids);

    container->unoccupied_ids = NULL;
    container->unoccupied_ids_count = 0;
    container->generations = NULL;
}

bool fn_alloc_handle(
    struct fn_handle_container* container,
    uint32_t*                   handle,
    uint32_t*                   index
) {
    assert(container && handle && index); 
    assert(container->unoccupied_ids);

    if(container->unoccupied_ids_count == 0)
        return false;

    uint32_t tmp = --container->unoccupied_ids_count;
    
    uint32_t tmp_handle = container->unoccupied_ids[tmp];
    uint8_t  tmp_gen = container->generations[tmp];

    *index = tmp_handle - 1;
    *handle = (tmp_gen << 24) | tmp_handle; 

    return true;
}

bool fn_dealloc_handle(
    struct fn_handle_container* container,
    uint32_t                    handle
) {
    assert(container);

    if(handle == 0)
        return false;

    uint8_t generation  = (handle >> 24); 
    uint32_t id         = ~(0x100 << 24) & handle;

    if(id == 0)
        return false;

    uint32_t idx = id - 1;

    if(container->generations[idx] != generation)
        return false;

    container->unoccupied_ids[container->unoccupied_ids_count++] = id;
    ++container->generations[idx];

    return true;
}

bool fn_check_handle(
    struct fn_handle_container* container,
    uint32_t                    handle,
    uint32_t*                   index
) {
    assert(container);

    if(handle == 0)
        return false;

    uint8_t generation  = (handle >> 24); 
    handle              &= ~(0x100 << 24);
    
    if(handle == 0)
        return false;

    uint32_t idx = handle - 1; 

    if(container->generations[idx] != generation)
        return false;

    if(index)
        *index = idx; 

    return true;
}

