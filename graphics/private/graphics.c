#include <fundament/graphics.h>
#include "graphics_common.h"

bool fn_init_graphics() {
    return fn__init_imp_graphics();
}

void fn_deinit_graphics() {
    fn__deinit_imp_graphics();
}

struct fn_swap_chain fn_create_swap_chain() {
    return (struct fn_swap_chain) { 0 };
}

void fn_destroy_swap_chain(struct fn_swap_chain swap_chain) {

}