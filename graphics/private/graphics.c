#include <fundament/graphics.h>
#include "graphics_common.h"

bool fn_init_graphics() {
    return fn__init_imp_graphics();
}

void fn_deinit_graphics() {
    fn__deinit_imp_graphics();
}
