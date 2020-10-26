#ifndef FUNDAMENT_GRAPHICS_H
#define FUNDAMENT_GRAPHICS_H

//==============================================================================
//                                  GRAPHICS
//
// Provides functions and structures utilize on the underlying graphics hardware
// for rendering and computations.
//
//==============================================================================

#include <fundament/types.h>

#include <stdbool.h>
#include <stdint.h>

struct fn_swap_chain{ uint32_t id; };
struct fn_texture{ uint32_t id; };
struct fn_command_list{ uint32_t id; };
struct fn_framebuffer{ uint32_t id; };

//
// Initializes the graphics module,
// allocating internal structures etc.
//
bool fn_init_graphics();

//
// Deinitializes the graphics module,
// releasing resources allocated through fn_init_graphics.
//
void fn_deinit_graphics();


struct fn_swap_chain_desc {
    fn_native_window_handle_t   window;
    uint32_t                    buffer_count;
    uint32_t                    width;
    uint32_t                    height;
};

//
// Creates a new swap chain for given window.
//
struct fn_swap_chain fn_create_swap_chain(
    const struct fn_swap_chain_desc* desc
);

//
// Destroys the given swap chain.
//
void fn_destroy_swap_chain(struct fn_swap_chain swap_chain);

//
// Acquires the current texture of the swap chain to render to.
//
struct fn_texture fn_swap_chain_current_texture(struct fn_swap_chain swap_chain);

void fn_swap_chain_present(struct fn_swap_chain swap_chain);

struct fn_framebuffer_desc {
    void*   vk_framebuffer;
};

struct fn_framebuffer fn_create_framebuffer(
    const struct fn_framebuffer_desc* desc
);

struct fn_texture_desc {

    // Vulkan specific
    struct {
        void*   image;
        void*   view;
    } vk;
};

//
// Creates a new texture.
//
struct fn_texture fn_create_texture(const struct fn_texture_desc* desc);

//
// Destroys a texture.
//
void fn_destroy_texture(struct fn_texture texture);

//
// Creates a new command list.
//
struct fn_command_list fn_create_command_list();

//
// Destroys a command list.
//
void fn_destroy_command_list(struct fn_command_list cmd_list);

void fn_encode_command_list(struct fn_command_list cmd_list);

void fn_commit_command_list(struct fn_command_list cmd_list);


struct fn_color {
    union {
        float rgba[4];
        struct {
            float r;
            float g;
            float b;
            float a;
        };
    };
};

enum fn_render_pass_action {
    fn_render_pass_action_none,
    fn_render_pass_action_clear,
    fn_render_pass_action_store
};

struct fn_render_pass{
    struct {
        struct fn_texture           texture;
        enum fn_render_pass_action  load_action;
        enum fn_render_pass_action  store_action;
        struct fn_color             clear_color;
    } color_attachments[8];
};

void fn_begin_render_pass(
    struct fn_command_list cmd_list,
    const struct fn_render_pass* pass
);

void fn_end_render_pass(struct fn_command_list cmd_list);

#endif  // FUNDAMENT_GRAPHICS_H

