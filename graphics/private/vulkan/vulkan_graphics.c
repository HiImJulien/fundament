#include "vulkan_graphics.h"
#include "../graphics_common.h"
#include <fundament/types.h>
#include <fundament/graphics.h>

#include "../graphics_backend.h"

#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32)
    #define VC_EXTRALEAN
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
    #include <vulkan/vulkan_win32.h>

    extern IMAGE_DOS_HEADER __ImageBase;
    static const HINSTANCE fn__g_instance = (HINSTANCE) &__ImageBase;
#endif

//==============================================================================
// Internal utility functions.
//==============================================================================

static const char* fn__required_vulkan_instance_exts[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
};

static const char* fn__required_vulkan_device_exts[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static bool fn__choose_suitable_adapter(
    VkPhysicalDevice* adapters,
    uint32_t count) {

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    uint32_t queue_family_count;
    uint32_t graphics_family_index = UINT32_MAX;
    uint32_t present_family_index  = UINT32_MAX;

    fn_native_window_handle_t dummy = GetDesktopWindow();
    VkSurfaceKHR dummy_surface;
    VkResult res = vkCreateWin32SurfaceKHR(
        fn__g_graphics_context.instance,
        &(VkWin32SurfaceCreateInfoKHR) {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .hinstance = GetModuleHandle(NULL),
            .hwnd = dummy
        },
        NULL,
        &dummy_surface
    );

    if(res != VK_SUCCESS) {
        printf("Failed to create dummy surface.\n");
        return false;
    }

    for(uint32_t it = 0; it < count; ++it) {
        vkGetPhysicalDeviceProperties(adapters[it], &properties);
        vkGetPhysicalDeviceFeatures(adapters[it], &features);

        const bool is_discrete = properties.deviceType
            == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;

        vkGetPhysicalDeviceQueueFamilyProperties(
            adapters[it],
            &queue_family_count,
            NULL
        );

        VkQueueFamilyProperties families[queue_family_count];
        vkGetPhysicalDeviceQueueFamilyProperties(
            adapters[it],
            &queue_family_count,
            families
        );

        if(!is_discrete)
            continue;

        for(uint32_t jt = 0; jt < queue_family_count; ++jt) {
            const bool has_queues = families[jt].queueCount > 0;
            const bool can_render = families[jt].queueFlags & VK_QUEUE_GRAPHICS_BIT;

            if(!(has_queues && can_render))
                continue;

            graphics_family_index = jt;
        }

        for(uint32_t jt = 0; jt < queue_family_count; ++jt) {
            VkBool32 supports_presenting = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(
                adapters[it],
                jt,
                dummy_surface,
                &supports_presenting
            );

            if(supports_presenting)
                present_family_index = jt;
        }

        if(graphics_family_index == UINT32_MAX
            || present_family_index == UINT32_MAX) {
            graphics_family_index = UINT32_MAX;
            present_family_index = UINT32_MAX;
            continue;
        }

        fn__g_graphics_context.adapter = adapters[it];
        fn__g_graphics_context.graphics_queue_index = graphics_family_index;
        fn__g_graphics_context.present_queue_index = present_family_index;
    }

    vkDestroySurfaceKHR(
        fn__g_graphics_context.instance,
        dummy_surface,
        NULL
    );

    return fn__g_graphics_context.adapter != VK_NULL_HANDLE;
}

//==============================================================================
// "Public" API.
//==============================================================================

bool fn__init_vulkan_graphics() {
    VkResult res = vkCreateInstance(
        &(VkInstanceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &(VkApplicationInfo) {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            },
            .enabledExtensionCount = 2,
            .ppEnabledExtensionNames = fn__required_vulkan_instance_exts
        },
        NULL,
        &fn__g_graphics_context.instance
    );

    if(res != VK_SUCCESS)
        return false;

    uint32_t adapter_count;
    res = vkEnumeratePhysicalDevices(
        fn__g_graphics_context.instance,
        &adapter_count,
        NULL
    );

    if(res != VK_SUCCESS) {
        fn__deinit_vulkan_graphics();
        return false;
    }

    VkPhysicalDevice adapters[adapter_count];
    vkEnumeratePhysicalDevices(
        fn__g_graphics_context.instance,
        &adapter_count,
        adapters
    );

    if(!fn__choose_suitable_adapter(adapters, adapter_count)) {
        fn__deinit_vulkan_graphics();
        return false;
    }

    float priorities = 1.0f;
    VkDeviceQueueCreateInfo queue_descs[2] = { 0 };
    queue_descs[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_descs[0].queueCount = 1;
    queue_descs[0].pQueuePriorities = &priorities;
    queue_descs[0].queueFamilyIndex = fn__g_graphics_context.graphics_queue_index;
    queue_descs[1] = queue_descs[0];
    queue_descs[1].queueFamilyIndex = fn__g_graphics_context.present_queue_index;
    
    res = vkCreateDevice(
        fn__g_graphics_context.adapter,
        &(VkDeviceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 2,
            .pQueueCreateInfos = queue_descs,
            .enabledExtensionCount = 1,
            .ppEnabledExtensionNames = fn__required_vulkan_device_exts
        },
        NULL,
        &fn__g_graphics_context.device
    );

    if(res != VK_SUCCESS) {
        fn__deinit_vulkan_graphics();
        return false;
    }

    vkGetDeviceQueue(
        fn__g_graphics_context.device,
        fn__g_graphics_context.graphics_queue_index,
        0,
        &fn__g_graphics_context.graphics_queue
    );

    vkGetDeviceQueue(
        fn__g_graphics_context.device,
        fn__g_graphics_context.present_queue_index,
        0,
        &fn__g_graphics_context.present_queue
    );

    VkSemaphoreCreateInfo sem_desc = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
    };

    vkCreateSemaphore(
        fn__g_graphics_context.device,
        &sem_desc,
        NULL,
        &fn__g_graphics_context.image_sem
    );

    vkCreateSemaphore(
        fn__g_graphics_context.device,
        &sem_desc,
        NULL,
        &fn__g_graphics_context.render_sem
    );

    return true;
}

void fn__deinit_vulkan_graphics() {
    if(fn__g_graphics_context.device != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(fn__g_graphics_context.device);
        vkDestroyDevice(fn__g_graphics_context.device, NULL);
        fn__g_graphics_context.device = VK_NULL_HANDLE;
    }

    if(fn__g_graphics_context.instance != VK_NULL_HANDLE) {
        vkDestroyInstance(fn__g_graphics_context.instance, NULL);
        fn__g_graphics_context.instance = VK_NULL_HANDLE;
    }
}

bool fn__create_vulkan_swap_chain(
    struct fn__vulkan_swap_chain* swap_chain,
    const struct fn_swap_chain_desc* desc
) {
    VkResult res = VK_SUCCESS;
#if defined(_WIN32)
    res = vkCreateWin32SurfaceKHR(
        fn__g_graphics_context.instance,
        &(VkWin32SurfaceCreateInfoKHR) {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .hwnd = desc->window,
            .hinstance = fn__g_instance
        },
        NULL,
        &swap_chain->surface
    );
#else
    #warning Unknown platform!
    return false;
#endif

    if(res != VK_SUCCESS)
        return false;

    VkBool32 can_present;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        fn__g_graphics_context.adapter,
        fn__g_graphics_context.present_queue_index,
        swap_chain->surface,
        &can_present
    );

    if(!can_present) {
        fn__destroy_vulkan_swap_chain(swap_chain);
        return false;
    }

    VkSurfaceCapabilitiesKHR surface_cap;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        fn__g_graphics_context.adapter,
        swap_chain->surface,
        &surface_cap
    );

    if(desc->buffer_count < surface_cap.minImageCount
        || desc->buffer_count > surface_cap.maxImageCount) {
        fn__destroy_vulkan_swap_chain(swap_chain);
        return false;
    }

    res = vkCreateSwapchainKHR(
        fn__g_graphics_context.device,
        &(VkSwapchainCreateInfoKHR) {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = swap_chain->surface,
            .minImageCount = desc->buffer_count,
            .imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
            .imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ,
            .imageExtent = (VkExtent2D) {
                .width = desc->width,
                .height = desc->height
            },
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR,
            .clipped = VK_TRUE
        },
        NULL,
        &swap_chain->swap_chain
    );

    if(res != VK_SUCCESS) {
        fn__destroy_vulkan_swap_chain(swap_chain);
        return false;
    }

    vkGetSwapchainImagesKHR(
        fn__g_graphics_context.device,
        swap_chain->swap_chain,
        &swap_chain->texture_count,
        NULL
    );

    VkImage images[swap_chain->texture_count];
    vkGetSwapchainImagesKHR(
        fn__g_graphics_context.device,
        swap_chain->swap_chain,
        &swap_chain->texture_count,
        images
    );

    VkImageView views[swap_chain->texture_count];
    VkImageViewCreateInfo view_desc = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        }
    };

    swap_chain->textures = malloc(
        sizeof(struct fn_texture) * swap_chain->texture_count
    );

    for(uint32_t it = 0; it < swap_chain->texture_count; ++it) {
        view_desc.image = images[it];

        vkCreateImageView(
            fn__g_graphics_context.device,
            &view_desc,
            NULL,
            &views[it]
        );

        swap_chain->textures[it] = fn_create_texture(
            &(struct fn_texture_desc) {
                .vk = {
                    .image = images[it],
                    .view = views[it]
                }
            }
        );
    }

    swap_chain->current_image = 0;

    return true;
}

void fn__destroy_vulkan_swap_chain(
    struct fn__vulkan_swap_chain* swap_chain
) {
    for(uint32_t it = 0;
        it < swap_chain->texture_count && swap_chain->textures;
        ++it) {
        fn_destroy_texture(swap_chain->textures[it]);
    }

    free(swap_chain->textures);
    swap_chain->textures = NULL;

    if(swap_chain->swap_chain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(
            fn__g_graphics_context.device,
            swap_chain->swap_chain,
            NULL
        );

        swap_chain->swap_chain = VK_NULL_HANDLE;
    }

    if(swap_chain->surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(
            fn__g_graphics_context.instance,
            swap_chain->surface,
            NULL
        );

        swap_chain->surface = VK_NULL_HANDLE;
    }
}

struct fn_texture fn__vulkan_swap_chain_current_texture(
    struct fn__vulkan_swap_chain* swap_chain
) {
    VkResult res = vkAcquireNextImageKHR(
        fn__g_graphics_context.device,
        swap_chain->swap_chain,
        0,
        fn__g_graphics_context.image_sem,
        VK_NULL_HANDLE,
        &swap_chain->current_image
    );

    const bool good =
        res == VK_SUCCESS
        || res == VK_NOT_READY
        || res == VK_SUBOPTIMAL_KHR;

    return good
        ? swap_chain->textures[swap_chain->current_image]
        : (struct fn_texture) { 0 };
}

bool fn__create_vulkan_texture(
    struct fn__vulkan_texture* texture,
    const struct fn_texture_desc* desc
) {
    if(desc->vk.image && desc->vk.view) {
        texture->image = desc->vk.image;
        texture->view = desc->vk.view;
        return true;
    }

    return false;
}

bool fn__create_vulkan_command_list(struct fn__vulkan_command_list* cmd) {
    VkCommandPoolCreateInfo pool_desc = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .queueFamilyIndex = fn__g_graphics_context.present_queue_index
    };

    VkResult res = vkCreateCommandPool(
        fn__g_graphics_context.device,
        &pool_desc,
        NULL,
        &cmd->command_pool
    );

    if(res != VK_SUCCESS)
        return false;

    VkCommandBufferAllocateInfo buffer_desc = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = cmd->command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1
    };

    res = vkAllocateCommandBuffers(
        fn__g_graphics_context.device,
        &buffer_desc,
        &cmd->command_buffer
    );

    if(res != VK_SUCCESS) {
        fn__destroy_vulkan_command_list(cmd);
        return false;
    }

    return true;
}

void fn__destroy_vulkan_command_list(struct fn__vulkan_command_list* cmd) {
    if(cmd->command_pool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(
            fn__g_graphics_context.device,
            cmd->command_pool,
            NULL
        );

        cmd->command_pool = VK_NULL_HANDLE;
        cmd->command_buffer = VK_NULL_HANDLE;
    }
}

void fn__encode_vulkan_command_list(struct fn__vulkan_command_list* cmd) {
    vkEndCommandBuffer(cmd->command_buffer);
}

void fn__commit_vulkan_command_list(struct fn__vulkan_command_list* cmd) {
    vkQueueSubmit(
        fn__g_graphics_context.graphics_queue,
        1,
        &(VkSubmitInfo) {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &cmd->command_buffer
        },
        NULL
    );
}

void fn__begin_vulkan_render_pass(
    struct fn__vulkan_command_list* cmd,
    const struct fn_render_pass* pass,
    fn__texture_t* textures[8]
) {
    vkBeginCommandBuffer(
        cmd->command_buffer,
        &(VkCommandBufferBeginInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT
        }
    );

    for(uint8_t it = 0; it < FN_MAX_ACTIVE_COLOR_ATTACHEMENTS; ++it) {
        const struct fn_color_attachment* ca = &pass->color_attachments[it];
        if(textures[it] == NULL || !ca->clear)
            continue;

        VkImageSubresourceRange range = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .levelCount = 1,
            .layerCount = 1
        };

        VkClearColorValue clear_color = {
            .float32 = {
                ca->clear_color.r,
                ca->clear_color.g,
                ca->clear_color.b,
                ca->clear_color.a,
            }
        };

        vkCmdClearColorImage(
            cmd->command_buffer,
            textures[it]->image,
            VK_IMAGE_LAYOUT_GENERAL,
            &clear_color,
            1,
            &range
        );
    }
}

struct fn__graphics_backend fn__g_vulkan_backend = {
    fn_graphics_backend_type_vulkan,
    &fn__init_vulkan_graphics,
    &fn__deinit_vulkan_graphics,

};