#include <fundament/window.h>
#include <fundament/event.h>
#include <fundament/graphics.h>

int main() {
    fn_init_window_module();

    struct fn_gfx_device device = fn_create_gfx_device();
    fn_destroy_gfx_device(device);


    fn_deinit_window_module();
    return 0;
}
