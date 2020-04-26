#include <fundament/fundament.h>
#include <fundament/gfx.h>

#include "./simple_triangle_shaders.h"

#include <stdlib.h>
#include <stdio.h>

const float vertices[] = {
    0.0f, 0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f
};

int main() {
    if(!fn_init()) {
        printf("Failed to initialize fundament.\n");
        return EXIT_FAILURE;
    }

    struct fn_window win = fn_window_create();
    fn_window_set_title(win, "Simple Window");
    fn_window_set_size(win, 300, 300);
    fn_window_set_visibility(win, true);

    struct fn_render_device device = fn_create_render_device();
    if(device.id == 0)
        printf("Failed to create device.\n");

    struct fn_swap_chain sc = fn_create_swap_chain(
        device,
        (struct fn_swap_chain_desc) {
            .format = fn_data_format_r8g8b8a8_unorm,
            .window = fn_window_handle(win),
            .width = 300,
            .height = 300
        }
    );

    if(sc.id == 0)
        printf("Failed to create swap chain.\n");

    struct fn_buffer verts = fn_create_buffer(
        device,
        (struct fn_buffer_desc) {
            .size = sizeof(float) * 9,
            .usage = fn_resource_usage_immutable,
            .bind_flags = fn_resource_bind_flags_vertex,
            .data = (const void*) vertices,
    });

    if(verts.id == 0)
        printf("Failed to create vertex buffer.\n");

    struct fn_shader vs = fn_create_shader(
        device,
        (struct fn_shader_desc) {
            .type = fn_shader_type_vertex,
            .byte_code = (const void*) vertex_shader,
            .byte_code_size = vertex_shader_size
        }
    );

    struct fn_shader ps = fn_create_shader(
        device,
        (struct fn_shader_desc) {
            .type = fn_shader_type_pixel,
            .byte_code = (const void*) pixel_shader,
            .byte_code_size = pixel_shader_size
        }
    );

    struct fn_pipeline pipe = fn_create_pipeline(
        device,
        (struct fn_pipeline_desc) {
            .vertex_shader = vs,
            .pixel_shader = ps,
            .topology = fn_topology_type_triangle_list,
            .layout[0] = {
                .format = fn_data_format_r32g32b32_float
            }
        } 
    );

    fn_apply_pipeline(device, pipe);

    fn_apply_bindings(
        device,
        (struct fn_buffer_binding) {
            .vertex_buffers[0] = {
                .slot = 0,
                .buffer = verts,
                .stride = sizeof(float) * 3,
                .offset = 0
            },
            .vertex_buffer_count = 1,
            .swap_chains[0] = sc
        }
    );

    struct fn_event ev = {0, };
    while(ev.type != fn_event_type_window_closed) {
        fn_clear(device, sc, 0.4f, 0.6f, 0.9f, 0.0f);
        fn_draw(device, 3, 0);
        fn_present(device, sc);
        fn_poll_event(&ev);
    }

    fn_window_destroy(win);
    return EXIT_SUCCESS;
}