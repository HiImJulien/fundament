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

    memset(
        fn__g_graphics_context.swap_chains,
        0,
        FN_WINDOW_CAPACITY * sizeof(fn__swap_chain_t)
    );

    memset(
        fn__g_graphics_context.textures,
        0,
        FN_TEXTURE_CAPACITY * sizeof(fn__texture_t)
    );

    memset(
        fn__g_graphics_context.command_lists,
        0,
        FN_COMMAND_LIST_CAPACITY * sizeof(fn__command_list_t)
    );

    fn_initialize_handle_pool(
        &fn__g_graphics_context.swap_chain_pool,
        FN_WINDOW_CAPACITY
    );

    fn_initialize_handle_pool(
        &fn__g_graphics_context.framebuffer_pool,
        FN_TEXTURE_CAPACITY
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

    VkResult res = vkQueuePresentKHR(
        fn__g_graphics_context.present_queue,
        &present_desc
    );

    if(res == VK_SUCCESS || res == VK_SUBOPTIMAL_KHR)
        printf("Noice!\n");
    else
        printf("FUUUUUCK\n");
}

struct fn_framebuffer fn_create_framebuffer(
    const struct fn_framebuffer_desc* desc
) {
    uint32_t index, handle;
    if(!fn_alloc_handle(
        &fn__g_graphics_context.framebuffer_pool,
        &handle,
        &index
        ))
        return (struct fn_framebuffer) { 0 };

    fn__framebuffer_t* ptr = &fn__g_graphics_context.framebuffers[index];
    if(!fn__create_imp_framebuffer(ptr, desc))
        return (struct fn_framebuffer) { 0 };

    return (struct fn_framebuffer) { handle };
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
    fn__command_list_t* cmd = fn__get_command_list(cmd_list);
    if(!cmd)
        return;

    vkEndCommandBuffer(cmd->command_buffer);
}

void fn_commit_command_list(struct fn_command_list cmd_list) {
    fn__command_list_t* cmd = fn__get_command_list(cmd_list);
    if(!cmd)
        return;

    VkResult res = vkQueueSubmit(
        fn__g_graphics_context.graphics_queue,
        1,
        &(VkSubmitInfo) {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &cmd->command_buffer
        },
        NULL
    );

    if(res != VK_SUCCESS)
        printf("Failed to submit queue.\n");
}

void fn_begin_render_pass(
    struct fn_command_list cmd_list,
    const struct fn_render_pass* pass
) {
    fn__command_list_t* cmd = fn__get_command_list(cmd_list);
    if(!cmd)
        return;

    VkResult res = vkBeginCommandBuffer(cmd->command_buffer, &(VkCommandBufferBeginInfo) {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
    });

    if(res != VK_SUCCESS)
        printf("Failed to begin!\n");

    for(uint8_t it = 0; it < 8; ++it) {
        if(pass->color_attachments[it].texture.id == 0)
            continue;

        VkImageSubresourceRange range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        };

        fn__texture_t* texture = fn__get_texture(pass->color_attachments[it].texture);
        vkCmdClearColorImage(
            cmd->command_buffer,
            texture->image,
            VK_IMAGE_LAYOUT_GENERAL,
            &(VkClearColorValue) {
                .float32[0] = pass->color_attachments[it].clear_color.rgba[0],
                .float32[1] = pass->color_attachments[it].clear_color.rgba[1],
                .float32[2] = pass->color_attachments[it].clear_color.rgba[2],
                .float32[3] = pass->color_attachments[it].clear_color.rgba[3],
            },
            1,
            &range
        );
    }

}

void fn_end_render_pass(struct fn_command_list cmd_list) {

}
