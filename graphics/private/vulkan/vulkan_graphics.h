#ifndef FUNDAMENT_VULKAN_GRAPHICS_H
#define FUNDAMENT_VULKAN_GRAPHICS_H

//------------------------------------------------------------------------------
//                            FORWARD DECLARATIONS
//------------------------------------------------------------------------------

struct fn_texture;
struct fn_texture_desc;
struct fn_swap_chain_desc;
struct fn_render_pass;

extern struct fn__graphics_backend fn__g_vulkan_backend;

//------------------------------------------------------------------------------
//                            INTERNAL STRUCTURES
//------------------------------------------------------------------------------

#include <stdbool.h>
#include <vulkan/vulkan.h>

typedef struct fn__vulkan_swap_chain fn__swap_chain_t;
struct fn__vulkan_swap_chain {
    VkSurfaceKHR            surface;
    VkSwapchainKHR          swap_chain;
    uint32_t                current_image;
    uint32_t                texture_count;
    struct fn_texture*      textures;
};

typedef struct fn__vulkan_texture fn__texture_t;
struct fn__vulkan_texture {
    bool            owned_by_swapchain;
    VkImage         image;
    VkImageView     view;
};

typedef struct fn__vulkan_command_list fn__command_list_t;
struct fn__vulkan_command_list {
    VkCommandPool   command_pool;
    VkCommandBuffer command_buffer;
};

bool fn__init_vulkan_graphics();
void fn__deinit_vulkan_graphics();

bool fn__create_vulkan_swap_chain(
    struct fn__vulkan_swap_chain* swap_chain,
    const struct fn_swap_chain_desc* desc
);

void fn__destroy_vulkan_swap_chain(struct fn__vulkan_swap_chain* swap_chain);

struct fn_texture fn__vulkan_swap_chain_current_texture(
    struct fn__vulkan_swap_chain* swap_chain
);

bool fn__create_vulkan_texture(
    struct fn__vulkan_texture* texture,
    const struct fn_texture_desc* desc
);

bool fn__create_vulkan_command_list(struct fn__vulkan_command_list* cmd);
void fn__destroy_vulkan_command_list(struct fn__vulkan_command_list* cmd);
void fn__encode_vulkan_command_list(struct fn__vulkan_command_list* cmd);
void fn__commit_vulkan_command_list(struct fn__vulkan_command_list* cmd);

void fn__begin_vulkan_render_pass(
    struct fn__vulkan_command_list* cmd,
    const struct fn_render_pass* pass,
    fn__texture_t* textures[8]
);

#define fn__init_imp_graphics               fn__init_vulkan_graphics
#define fn__deinit_imp_graphics             fn__deinit_vulkan_graphics

#define fn__create_imp_swap_chain           fn__create_vulkan_swap_chain
#define fn__destroy_imp_swap_chain          fn__destroy_vulkan_swap_chain
#define fn__imp_swap_chain_current_texture  fn__vulkan_swap_chain_current_texture

#define fn__create_imp_texture              fn__create_vulkan_texture

#define fn__create_imp_command_list         fn__create_vulkan_command_list
#define fn__destroy_imp_command_list        fn__destroy_vulkan_command_list
#define fn__encode_imp_command_list         fn__encode_vulkan_command_list
#define fn__commit_imp_command_list         fn__commit_vulkan_command_list

#define fn__begin_imp_render_pass           fn__begin_vulkan_render_pass

#endif  // FUNDAMENT_VULKAN_GRAPHICS_H
