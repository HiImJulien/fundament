#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <fundament/handle_container.h>

int main() {
    struct fn_handle_container container;

    if(!fn_initialize_handle_container(&container, 32)) {
        printf("Failed to initialize container.\n");
        return EXIT_FAILURE;
    } 

    for(uint32_t it = 0; it < 32; ++it) {
        uint32_t handle, index;
        if(!fn_alloc_handle(&container, &handle, &index)) {
            printf("Failed to allocate handle.\n");
            return EXIT_FAILURE;
        }

        printf("Handle: %"PRIu32" Index: %"PRIu32"\n",
            handle, index);

        if(!fn_check_handle(&container, handle, NULL)) {
            printf("Failed to check allocated handle.\n");
            return EXIT_FAILURE;
        }
    }

    for(uint32_t it = 0; it < 32; ++it)
        if(!fn_dealloc_handle(&container, it + 1)) {
            printf("Failed to deallocate handle.\n");
            return EXIT_FAILURE;
        }

    printf("--- Allocating handles; expecting generations to appear.\n");

    for(uint32_t it = 0; it < 32; ++it) {
        uint32_t handle, index;
        if(!fn_alloc_handle(&container, &handle, &index)) {
            printf("Failed to allocate handle.\n");
            return EXIT_FAILURE;
        }

        printf("Handle: %"PRIu32" Index: %"PRIu32"\n",
            handle, index);

        if(!fn_check_handle(&container, handle, NULL)) {
            printf("Failed to check allocated handle.\n");
            return EXIT_FAILURE;
        }
    }

    printf("--- Tests passed.\n");

    return EXIT_SUCCESS;
}
