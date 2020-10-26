#include <fundament/graphics.h>
#include "graphics_common.h"

#include <string.h>
#include <stdio.h>

// --- utility
static fn__command_list_t* fn__get_command_list(struct fn_command_list cmd) {
    uint32_t index;
    if(!fn_check_handle(
        &fn__g_graphics_context.command_list_pool,
        cmd.id,
        &index
        ))
        return NULL;

    return &fn__g_graphics_context.command_lists[index];
}

static fn__texture_t* fn__get_texture(struct fn_texture texture) {
    uint32_t index;
    if(!fn_check_handle(
        &fn__g_graphics_context.texture_pool,
        texture.id,
        &index
        ))
        return NULL;

    return &fn__g_graphics_context.textures[index];
}

bool fn_init_graphics() {
    if(!fn__init_imp_graphics())
        return false;

    fn_initialize_handle_pool(
        &fn__g_graphics_context.swap_chain_pool,
        FN_WINDOW_CAPACITY
    );

    fn_initialize_handle_pool(
        &fn__g_graphics_context.texture_pool,
        FN_TEXTURE_CAPACITY
    );

    fn_initialize_handle_pool(
        &fn__g_graphics_context.command_list_pool,
        FN_COMMAND_LIST_CAPACITY
    );

    return true;
}

void fn_deinit_graphics() {
    fn__deinit_imp_graphics();

    fn_deinitialize_handle_pool(&fn__g_graphics_context.swap_chain_pool);
    fn_deinitialize_handle_pool(&fn__g_graphics_context.texture_pool);
    fn_deinitialize_handle_pool(&fn__g_graphics_context.command_list_pool);
}

struct fn_swap_chain fn_create_swap_chain(
    const struct fn_swap_chain_desc* desc
    ) {
    uint32_t handle, index;
    if(!fn_alloc_handle(
        &fn__g_graphics_context.swap_chain_pool,
        &handle,
        &index
        ))
        return (struct fn_swap_chain) { 0 };

    fn__swap_chain_t* ptr = &fn__g_graphics_context.swap_chains[index];
    if(!fn__create_imp_swap_chain(ptr, desc))
        return (struct fn_swap_chain) { 0 };

    return (struct fn_swap_chain) { handle };
}

void fn_destroy_swap_chain(struct fn_swap_chain swap_chain) {
    uint32_t index;
    if(!fn_check_handle(
        &fn__g_graphics_context.swap_chain_pool,
        swap_chain.id,
        &index))
        return;

    fn__swap_chain_t* ptr = &fn__g_graphics_context.swap_chains[index];
    fn__destroy_imp_swap_chain(ptr);

    fn_dealloc_handle(&fn__g_graphics_context.swap_chain_pool, swap_chain.id);
}

struct fn_texture fn_swap_chain_current_texture(struct fn_swap_chain swap_chain) {
    uint32_t index;
    if(!fn_check_handle(
            &fn__g_graphics_context.swap_chain_pool,
            swap_chain.id,
            &index
        ))
        return (struct fn_texture) { 0 };

    fn__swap_chain_t* ptr = &fn__g_graphics_context.swap_chains[index];
    return fn__imp_swap_chain_current_texture(ptr);
}

void fn_swap_chain_present(struct fn_swap_chain swap_chain) {
    uint32_t index;
    if(!fn_check_handle(
            &fn__g_graphics_context.swap_chain_pool,
            swap_chain.id,
            &index
        ))
        return;

    fn__swap_chain_t* ptr = &fn__g_graphics_context.swap_chains[index];

    VkPresentInfoKHR present_desc = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .swapchainCount = 1,
        .pSwapchains = &ptr->swap_chain,
        .pImageIndices = &ptr->current_image
    };

    vkQueuePresentKHR(
        fn__g_graphics_context.present_queue,
        &present_desc
    );
}

struct fn_texture fn_create_texture(const struct fn_texture_desc* desc) {
    uint32_t index, handle;
    if(!fn_alloc_handle(
        &fn__g_graphics_context.texture_pool,
        &handle,
        &index
        ))
        return (struct fn_texture) { 0 };

    fn__texture_t* ptr = &fn__g_graphics_context.textures[index];
    if(!fn__create_imp_texture(ptr, desc))
        return (struct fn_texture) { 0 };

    return (struct fn_texture) { handle };
}

void fn_destroy_texture(struct fn_texture texture) {

}

struct fn_shader fn_create_shader(const struct fn_shader_desc* desc) {
    uint32_t handle, index;
    if(!fn_alloc_handle(
        &fn__g_graphics_context.command_list_pool,
        &handle,
        &index
        ))
        return (struct fn_shader) { 0 };

    fn__shader_t* ptr = &fn__g_graphics_context.shaders[index];
    if(!fn__create_imp_shader(ptr, desc))
        return (struct fn_shader) { 0 };

    return (struct fn_shader) { handle };
}

struct fn_command_list fn_create_command_list() {
    uint32_t handle, index;
    if(!fn_alloc_handle(
        &fn__g_graphics_context.command_list_pool,
        &handle,
        &index
        ))
        return (struct fn_command_list) { 0 };

    fn__command_list_t* ptr = &fn__g_graphics_context.command_lists[index];
    if(!fn__create_imp_command_list(ptr))
        return (struct fn_command_list) { 0 };

    return (struct fn_command_list) { handle };
}

void fn_destroy_command_list(struct fn_command_list cmd_list) {
    uint32_t index;
    if(!fn_check_handle(
        &fn__g_graphics_context.command_list_pool,
        cmd_list.id,
        &index
        ))
        return;

    fn__command_list_t* ptr = &fn__g_graphics_context.command_lists[index];
    fn__destroy_imp_command_list(ptr);
}

void fn_encode_command_list(struct fn_command_list cmd_list) {
    fn__command_list_t* ptr = fn__get_command_list(cmd_list);
    if(ptr)
        fn__encode_imp_command_list(ptr);

}

void fn_commit_command_list(struct fn_command_list cmd_list) {
    fn__command_list_t* ptr = fn__get_command_list(cmd_list);
    if(ptr)
        fn__commit_imp_command_list(ptr);
}

void fn_begin_render_pass(
    struct fn_command_list cmd_list,
    const struct fn_render_pass* pass
) {
    fn__command_list_t* cmd = fn__get_command_list(cmd_list);
    fn__texture_t* textures[FN_MAX_ACTIVE_COLOR_ATTACHMENTS];


    for(uint8_t it = 0; it < FN_MAX_ACTIVE_COLOR_ATTACHMENTS; ++it) {
        textures[it] = NULL;
        if(pass->color_attachments[it].texture.id != 0)
            textures[it] = fn__get_texture(pass->color_attachments[it].texture);
    }

    fn__begin_imp_render_pass(cmd, pass, textures);
}
