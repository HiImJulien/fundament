#include "vulkan_graphics.h"
#include "../graphics_common.h"
#include <fundament/types.h>

#include <stdio.h>

#if defined(_WIN32)
    #include <Windows.h>
    #include <vulkan/vulkan_win32.h>
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
    res = vkCreateDevice(
        fn__g_graphics_context.adapter,
        &(VkDeviceCreateInfo) {
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = 1,
            .pQueueCreateInfos = &(VkDeviceQueueCreateInfo ) {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = fn__g_graphics_context.graphics_queue_index,
                .queueCount = 1,
                .pQueuePriorities = &priorities,
            },
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
