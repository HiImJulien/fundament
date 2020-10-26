#include <fundament/window.h>
#include <fundament/event.h>
#include <fundament/graphics.h>

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    fn_init_graphics();
    fn_init_window();

    struct fn_window window = fn_create_window();
    fn_window_set_title(window, "Graphics Sample.");
    fn_window_set_size(window, 800, 600);
    fn_window_set_visible(window, true);

    printf("Initialized window.\n");

    struct fn_swap_chain sc = fn_create_swap_chain(
        &(struct fn_swap_chain_desc) {
            .buffer_count = 2,
            .window = fn_window_handle(window),
            .width = 800,
            .height = 600,
        }
    );

    if (sc.id == 0)
        printf("Failed to create swap chain.\n");

    struct fn_command_list cmd = fn_create_command_list();
    fn_begin_render_pass(cmd, &(struct fn_render_pass) {
        .color_attachments[0] = {
            .texture = fn_swap_chain_current_texture(sc),
            .clear_color = {0.4f, 0.6f, 0.9f, 0.0f}
        }
    });

    if(cmd.id == 0)
        printf("Failed to create command list.\n");

    struct fn_event ev = { 0 };
    while (ev.type != fn_event_type_closed) {
        struct fn_texture current_drawable = fn_swap_chain_current_texture(sc);
        if(current_drawable.id == 0)
            printf("Failed to retrieve the current drawable.\n");

        fn_begin_render_pass(cmd, &(struct fn_render_pass) {
            .color_attachments[0] = {
                .texture = current_drawable,
                .clear = true,
                .clear_color = {0.4f, 0.6f, 0.9f, 0.0f}
            }
        });

        fn_encode_command_list(cmd);
        fn_commit_command_list(cmd);
        fn_swap_chain_present(sc);

        fn_poll_events(&ev);
    }

    // TODO: fn_dealloc_handle causes a HEAP_CORRUPTION?
    // fn_destroy_swap_chain(sc);
    fn_destroy_command_list(cmd);

    fn_deinit_window();
    fn_deinit_graphics();
    return EXIT_SUCCESS;
}