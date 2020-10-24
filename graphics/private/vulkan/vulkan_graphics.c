#include "vulkan_graphics.h"
#include "../graphics_common.h"

#include <stdlib.h>

#include <vulkan/vulkan.h>

#if defined(_WIN32)
    #define VC_EXTRALEAN
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>

    extern IMAGE_DOS_HEADER __ImageBase;
    static const HINSTANCE fn__g_instance = (HINSTANCE) &__ImageBase;

    #include <vulkan/vulkan_win32.h>
#endif

#include <stdio.h>
#include <inttypes.h>

static const char* fn__g_required_extensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
};

static const char* fn__g_device_extensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

bool fn__init_vulkan_graphics() {
    if(fn__g_graphics_context.instance
        && fn__g_graphics_context.physical_device
        && fn__g_graphics_context.device)
        return true;

    VkInstanceCreateInfo instance_desc = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &(struct VkApplicationInfo) {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_VERSION_1_0
        },
        .enabledExtensionCount = 2,
        .ppEnabledExtensionNames = fn__g_required_extensions
    };

    if(VK_SUCCESS != vkCreateInstance(
        &instance_desc,
        NULL,
        &fn__g_graphics_context.instance))
        return false;

    uint32_t device_count = 0;
    if(VK_SUCCESS != vkEnumeratePhysicalDevices(
        fn__g_graphics_context.instance,
        &device_count,
        NULL
        ))
        return false;

    VkPhysicalDevice devices[device_count];
    vkEnumeratePhysicalDevices(
        fn__g_graphics_context.instance,
        &device_count,
        devices
    );

    for(uint32_t it = 0; it < device_count; ++it) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(
            devices[it],
            &props
        );

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(
            devices[it],
            &features
        );

        if(props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            fn__g_graphics_context.physical_device = devices[it];
            break;
        }
    }

    if(!fn__g_graphics_context.physical_device) {
        fn__deinit_vulkan_graphics();
        return false;
    }

    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        fn__g_graphics_context.physical_device,
        &queue_family_count,
        NULL
    );

    printf("Device has %"PRIu32" queue families.\n", queue_family_count);

    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(
        fn__g_graphics_context.physical_device,
        &queue_family_count,
        queue_families
    );

    uint32_t index = 0;
    for(uint32_t it  = 0; it < queue_family_count; ++it) {
        if(queue_families[it].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            index = it;

            // TODO: I'll be nasty and assume, that this queue can present
            // as well.
        }
    }

    float queue_priority = 1.f;
    VkDeviceCreateInfo device_desc = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pQueueCreateInfos = &(VkDeviceQueueCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .queueFamilyIndex = index,
            .queueCount = 1,
            .pQueuePriorities = &queue_priority
        },
        .queueCreateInfoCount = 1,
        .enabledExtensionCount = 1,
        .ppEnabledExtensionNames = fn__g_device_extensions
    };

    if(VK_SUCCESS != vkCreateDevice(
        fn__g_graphics_context.physical_device,
        &device_desc,
        NULL,
        &fn__g_graphics_context.device
        ))
        return false;

    vkGetDeviceQueue(
        fn__g_graphics_context.device,
        index,
        0,
        &fn__g_graphics_context.queue
    );

    VkSemaphoreCreateInfo sem_desc = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        NULL,
        0
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

    vkCreateCommandPool(
        fn__g_graphics_context.device,
        &(VkCommandPoolCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .queueFamilyIndex = fn__g_graphics_context.queue_family_index
        },
        NULL,
        &fn__g_graphics_context.cmd_pool
    );

    vkAllocateCommandBuffers(
        fn__g_graphics_context.device,
        &(VkCommandBufferAllocateInfo) {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = fn__g_graphics_context.cmd_pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        },
        &fn__g_graphics_context.cmd_buffer
    );

    return true;
}

void fn__deinit_vulkan_graphics() {
    vkDestroyDevice(
        fn__g_graphics_context.device,
        NULL
    );

    fn__g_graphics_context.device = VK_NULL_HANDLE;

    vkDestroyInstance(
        fn__g_graphics_context.instance,
        NULL
    );

    fn__g_graphics_context.instance = VK_NULL_HANDLE;
}

bool fn__create_vulkan_surface(
    fn_native_window_handle_t window,
    struct fn__surface* surface
) {
#if (_WIN32)
    VkWin32SurfaceCreateInfoKHR surface_desc = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hwnd = window,
        .hinstance = fn__g_instance
    };

    if(VK_SUCCESS != vkCreateWin32SurfaceKHR(
        fn__g_graphics_context.instance,
        &surface_desc,
        NULL,
        &surface->surface))
            return false;
#else
    return false;
#endif

    VkBool32 can_present = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(
        fn__g_graphics_context.physical_device,
        fn__g_graphics_context.queue_family_index,
        surface->surface,
        &can_present
    );

    if(!can_present) {
        vkDestroySurfaceKHR(
            fn__g_graphics_context.instance,
            surface->surface,
            NULL
        );

        return false;
    }

    VkSurfaceFormatKHR surface_format = {
        .format = VK_FORMAT_R8G8B8A8_SRGB,
        .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    };

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    VkExtent2D  extent = {
        .width = 800,
        .height = 600
    };

    VkSwapchainCreateInfoKHR swapchain_desc = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = surface->surface,
        .minImageCount = 2,
        .imageFormat = surface_format.format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = TRUE,
        .oldSwapchain = VK_NULL_HANDLE
    };

    if(vkCreateSwapchainKHR(
        fn__g_graphics_context.device,
        &swapchain_desc,
        NULL,
        &surface->swapchain) != VK_SUCCESS)
        return false;

    vkGetSwapchainImagesKHR(
        fn__g_graphics_context.device,
        surface->swapchain,
        &surface->image_count,
        NULL
    );

    surface->images = malloc(sizeof(VkImage) * surface->image_count);
    vkGetSwapchainImagesKHR(
        fn__g_graphics_context.device,
        surface->swapchain,
        &surface->image_count,
        surface->images
    );

    return can_present;
}

void fn__destroy_vulkan_surface(struct fn__surface* surface) {
    vkDestroySurfaceKHR(
        fn__g_graphics_context.instance,
        surface->surface,
        NULL
    );

    surface->surface = NULL;
}

void fn__present_vulkan_surface(struct fn__surface* surface) {
    uint32_t image_index;
    VkResult res = vkAcquireNextImageKHR(
        fn__g_graphics_context.device,
        surface->swapchain,
        UINT64_MAX,
        fn__g_graphics_context.image_sem,
        VK_NULL_HANDLE,
        &image_index
    );

    if(res != VK_SUCCESS)
        printf("vkAcquireNextImageKHR didn't return VK_SUCCESS!\n");

    VkPipelineStageFlags  stage_mask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    VkSubmitInfo submit_info = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,
        NULL,
        1,
        &fn__g_graphics_context.image_sem,
        &stage_mask,
        1,
        &fn__g_graphics_context.cmd_buffer,
        1,
        &fn__g_graphics_context.render_sem
    };

    res = vkQueueSubmit(
        fn__g_graphics_context.queue,
        1,
        &submit_info,
        VK_NULL_HANDLE
    );

    if(res != VK_SUCCESS)
        printf("vkQueueSubmit failed.\n");
}