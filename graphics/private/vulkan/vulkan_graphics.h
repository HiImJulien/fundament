#ifndef FUNDAMENT_VULKAN_GRAPHICS_H
#define FUNDAMENT_VULKAN_GRAPHICS_H

#include <stdbool.h>

#include <vulkan/vulkan.h>

bool fn__init_vulkan_graphics();
void fn__deinit_vulkan_graphics();

struct fn_texture;
struct fn_framebuffer;
struct fn_swap_chain_desc;
struct fn__vulkan_swap_chain {
    VkSurfaceKHR            surface;
    VkSwapchainKHR          swap_chain;
    uint32_t                current_image;
    uint32_t                texture_count;
    struct fn_texture*      textures;
    struct fn_framebuffer*  framebuffers;
};

bool fn__create_vulkan_swap_chain(
    struct fn__vulkan_swap_chain* swap_chain,
    const struct fn_swap_chain_desc* desc
);

void fn__destroy_vulkan_swap_chain(struct fn__vulkan_swap_chain* swap_chain);

struct fn_texture fn__vulkan_swap_chain_current_texture(
    struct fn__vulkan_swap_chain* swap_chain
);

struct fn_framebuffer_desc;
struct fn__vulkan_framebuffer {
    VkFramebuffer   framebuffer;
};

bool fn__create_vulkan_framebuffer(
    struct fn__vulkan_framebuffer* framebuffer,
    const struct fn_framebuffer_desc* desc
);

struct fn_texture_desc;
struct fn__vulkan_texture {
    VkImage         image;
    VkImageView     view;
};

bool fn__create_vulkan_texture(
    struct fn__vulkan_texture* texture,
    const struct fn_texture_desc* desc
);

struct fn__vulkan_command_list {
    VkCommandPool   command_pool;
    VkCommandBuffer command_buffer;
};

bool fn__create_vulkan_command_list(struct fn__vulkan_command_list* cmd);
void fn__destroy_vulkan_command_list(struct fn__vulkan_command_list* cmd);

#define fn__init_imp_graphics               fn__init_vulkan_graphics
#define fn__deinit_imp_graphics             fn__deinit_vulkan_graphics
#define fn__create_imp_swap_chain           fn__create_vulkan_swap_chain
#define fn__destroy_imp_swap_chain          fn__destroy_vulkan_swap_chain
#define fn__imp_swap_chain_current_texture  fn__vulkan_swap_chain_current_texture
#define fn__create_imp_framebuffer          fn__create_vulkan_framebuffer
#define fn__create_imp_texture              fn__create_vulkan_texture
#define fn__create_imp_command_list         fn__create_vulkan_command_list
#define fn__destroy_imp_command_list        fn__destroy_vulkan_command_list

typedef struct fn__vulkan_swap_chain fn__swap_chain_t;
typedef struct fn__vulkan_framebuffer fn__framebuffer_t;
typedef struct fn__vulkan_texture fn__texture_t;
typedef struct fn__vulkan_command_list fn__command_list_t;

#endif  // FUNDAMENT_VULKAN_GRAPHICS_H
