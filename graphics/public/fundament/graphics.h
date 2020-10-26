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
#include <fundament/color.h>

#include <stdbool.h>
#include <stdint.h>

enum fn_graphics_backend_type {
    fn_graphics_backend_type_none,
    fn_graphics_backend_type_vulkan,
    fn_graphics_backend_type_metal,
    fn_graphics_backend_type_d3d12
};

struct fn_swap_chain{ uint32_t id; };
struct fn_texture{ uint32_t id; };
struct fn_command_list{ uint32_t id; };
struct fn_shader{ uint32_t id; };

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

//
// Encodes the contents of the command list for usage later.
//
void fn_encode_command_list(struct fn_command_list cmd_list);

//
// Commits the command list to be processed by the GPU.
//
void fn_commit_command_list(struct fn_command_list cmd_list);

struct fn_color_attachment {
    struct  fn_texture              texture;
    bool                            clear;
    union   fn_color                clear_color;
};

struct fn_render_pass{
    struct fn_color_attachment     color_attachments[FN_MAX_ACTIVE_COLOR_ATTACHMENTS];
};

void fn_begin_render_pass(
    struct fn_command_list cmd_list,
    const struct fn_render_pass* pass
);


#endif  // FUNDAMENT_GRAPHICS_H

