#include "vulkan_graphics.h"
#include "../graphics_common.h"

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
        }
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
        .queueCreateInfoCount = 1
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

    return VK_SUCCESS == vkCreateWin32SurfaceKHR(
        fn__g_graphics_context.instance,
        &surface_desc,
        NULL,
        &surface->native
    );
#else
    return false;
#endif
}

void fn__destroy_vulkan_surface(struct fn__surface* surface) {
    vkDestroySurfaceKHR(
        fn__g_graphics_context.instance,
        surface->native,
        NULL
    );
}