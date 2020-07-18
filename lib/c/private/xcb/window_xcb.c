#include "window_xcb.h"
#include <fundament/event.h>
#include "../input_common.h"
#include "../window_common.h"
#include "input_xcb.h"
#include "input_key_map_xcb.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#include <xcb/xinput.h>

// This atom is used to attach the window id,
// which the again is used to by the event handler
// to determine the exact window.
static xcb_atom_t atom_fundament_id = 0;

static bool xinput_present = false;
static uint8_t xinput_opcode = 0;
static uint16_t xinput_version = (((uint8_t) XCB_INPUT_MAJOR_VERSION << 8) |
                                  (uint8_t) XCB_INPUT_MINOR_VERSION);

static uint32_t get_fundament_id_from_window(fn_native_window_handle_t handle)
{
    xcb_get_property_cookie_t id_cookie = xcb_get_property(
        fn__g_window_context.connection, 
        false, 
        handle, 
        atom_fundament_id, 
        XCB_ATOM_INTEGER, 
        0, 
        1
    );

    xcb_get_property_reply_t* reply = xcb_get_property_reply(
        fn__g_window_context.connection, 
        id_cookie, 
        NULL
    );

    xcb_flush(fn__g_window_context.connection);

    uint32_t idx = *(uint32_t*) xcb_get_property_value(reply);
    xcb_flush(fn__g_window_context.connection);
    free(reply);

    return idx;
}

void fn__imp_init_window_context()
{
    Display* dpy = XOpenDisplay(NULL);
    fn__g_window_context.display = dpy; 
    fn__g_window_context.connection = XGetXCBConnection(dpy); 

    XSetEventQueueOwner(dpy, XCBOwnsEventQueue);

    fn__g_window_context.screen = xcb_setup_roots_iterator(
        xcb_get_setup(fn__g_window_context.connection)
    ).data;

    xcb_intern_atom_cookie_t protocols_cookie = xcb_intern_atom(
        fn__g_window_context.connection, 1, 12, "WM_PROTOCOLS"
    );

    xcb_intern_atom_reply_t* protocols_reply = xcb_intern_atom_reply(
        fn__g_window_context.connection, protocols_cookie, 0
    );

    xcb_intern_atom_cookie_t delete_window_cookie = xcb_intern_atom(
        fn__g_window_context.connection, 1, 16, "WM_DELETE_WINDOW"
    );

    xcb_intern_atom_reply_t* delete_window_reply = xcb_intern_atom_reply(
        fn__g_window_context.connection, delete_window_cookie, 0
    );

    xcb_intern_atom_cookie_t id_cookie = xcb_intern_atom(
        fn__g_window_context.connection, 0, 12, "FUNDAMENT_ID"
    );

    xcb_intern_atom_reply_t* id_cookie_reply = xcb_intern_atom_reply(
        fn__g_window_context.connection, id_cookie, 0
    );

    // Check whether XInput is supported.
    xcb_query_extension_cookie_t qxi_cookie = xcb_query_extension(
        fn__g_window_context.connection, 15, "XInputExtension"
    );

    xcb_query_extension_reply_t* qxi_reply = xcb_query_extension_reply(
        fn__g_window_context.connection, qxi_cookie, NULL
    );

    xcb_input_get_extension_version_cookie_t xiv_cookie = 
        xcb_input_get_extension_version(
            fn__g_window_context.connection, 15, "XInputExtension"
    );

    xcb_input_get_extension_version_reply_t* xiv_reply = 
        xcb_input_get_extension_version_reply(
        fn__g_window_context.connection, xiv_cookie, NULL
    );

    xcb_flush(fn__g_window_context.connection);

    fn__g_window_context.atom_protocols = protocols_reply->atom;
    fn__g_window_context.atom_delete_window = delete_window_reply->atom;
    // TODO: Replace with actual hashmap.
    atom_fundament_id = id_cookie_reply->atom;

    const uint16_t reported_version =
        (xiv_reply->server_major << 8) | xiv_reply->server_minor;

    if(qxi_reply->present && xinput_version >= reported_version) {
        fn__g_window_context.has_xinput = true;
        fn__g_window_context.opcode_xinput = qxi_reply->major_opcode;

        xcb_input_event_mask_t head = {
            .deviceid= XCB_INPUT_DEVICE_ALL, 
            .mask_len= sizeof(xcb_input_xi_event_mask_t) / sizeof(uint32_t)
        };

        xcb_input_xi_select_events(
            fn__g_window_context.connection, 
            fn__g_window_context.screen->root, 
            1, 
            &head
        );

        xcb_flush(fn__g_window_context.connection);
    }

    free(protocols_reply);
    free(delete_window_reply);
    free(id_cookie_reply);
    free(qxi_reply);
}

fn_native_window_handle_t fn__imp_create_window(uint32_t index)
{
    fn_native_window_handle_t handle = xcb_generate_id(
        fn__g_window_context.connection
    );

    const uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    const uint32_t mask_values[] = {
        fn__g_window_context.screen->white_pixel,
        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS |
        XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
        XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
        XCB_EVENT_MASK_STRUCTURE_NOTIFY};

    xcb_create_window(
        fn__g_window_context.connection, 
        XCB_COPY_FROM_PARENT, 
        handle, 
        fn__g_window_context.screen->root, 
        0, 
        0, 
        150, 
        150,
        10, 
        XCB_WINDOW_CLASS_INPUT_OUTPUT, 
        fn__g_window_context.screen->root_visual, 
        mask,
        mask_values
    );

    xcb_change_property(
        fn__g_window_context.connection, 
        XCB_PROP_MODE_REPLACE, 
        handle, 
        fn__g_window_context.atom_protocols, 
        4, 
        32, 
        1,
        &fn__g_window_context.atom_delete_window
    );

    xcb_change_property(
        fn__g_window_context.connection, 
        XCB_PROP_MODE_REPLACE, 
        handle, 
        atom_fundament_id,
        XCB_ATOM_INTEGER, 
        32, 
        1, 
        &index
    );

    xcb_map_window(fn__g_window_context.connection, handle);
    xcb_flush(fn__g_window_context.connection);

    if(fn__g_window_context.has_xinput) {
        xcb_input_event_mask_t head = {
            .deviceid= XCB_INPUT_DEVICE_ALL, 
            .mask_len= sizeof(xcb_input_xi_event_mask_t) / sizeof(uint32_t)
        };

        uint32_t* mask = xcb_input_event_mask_mask(&head);
        mask[0] = XCB_INPUT_XI_EVENT_MASK_KEY_PRESS |
                  XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE |
                  XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS |
                  XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE;

        xcb_input_xi_select_events(
            fn__g_window_context.connection, 
            handle, 
            1, 
            &head
        );

        xcb_flush(fn__g_window_context.connection);
    }

    return handle;
}

void fn__imp_destroy_window(fn_native_window_handle_t handle)
{
    xcb_destroy_window(fn__g_window_context.connection, handle);
    xcb_flush(fn__g_window_context.connection);
}

void fn__imp_window_set_size(
    fn_native_window_handle_t handle, uint32_t width, uint32_t height
)
{
    const uint32_t values[] = {width, height};

    xcb_configure_window(
        fn__g_window_context.connection, 
        handle, 
        XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
        values
    );

    xcb_flush(fn__g_window_context.connection);
}

void fn__imp_window_set_title(
    fn_native_window_handle_t handle, const char* title
)
{
    // Sets the window's title
    xcb_change_property(
        fn__g_window_context.connection, 
        XCB_PROP_MODE_REPLACE, 
        handle, 
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING, 
        8, 
        strlen(title), 
        title
    );

    // Sets the title of the window icon?
    // Dunno what that is, perhaps because
    // I am using i3?
    xcb_change_property(
        fn__g_window_context.connection, 
        XCB_PROP_MODE_REPLACE, 
        handle, 
        XCB_ATOM_WM_ICON_NAME,
        XCB_ATOM_STRING, 
        8, 
        strlen(title), 
        title
    );

    xcb_flush(fn__g_window_context.connection);
}

void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle, bool visible
)
{
    // I am currently not satisfied with this
    // solution, since an 'unmapped' window
    // doesn't allow many operations.
    // I believe drawing to may cause crashes,
    // altough I am not sure.

    if(visible)
        xcb_map_window(fn__g_window_context.connection, handle);
    else
        xcb_unmap_window(fn__g_window_context.connection, handle);

    xcb_flush(fn__g_window_context.connection);
}

static inline int32_t fp1616_to_int32(xcb_input_fp1616_t val)
{
    return (int32_t) ((float) val / (float) (1 << 16));
}

static void process_xinput_event(xcb_ge_generic_event_t* gev)
{
    switch(gev->event_type) {
        case XCB_INPUT_KEY_PRESS:
        case XCB_INPUT_KEY_RELEASE: {
            const bool is_press = gev->event_type == XCB_INPUT_KEY_PRESS;
            xcb_input_device_key_press_event_t* ev = (xcb_input_device_key_press_event_t*) gev;

            // The values of ev->child seem to be 'offset' by 8
            // when comparing to linux key codes.
            const enum fn_key key = fn__imp_map_xcb_key(
                (uint32_t) ev->child - 8
            );

            const char letter = fn__imp_translate_key(
                fn__g_window_context.display, (uint32_t) ev->child
            );

            fn__notify_key_changed(key, letter, is_press);
        } break;

        case XCB_INPUT_BUTTON_PRESS:
        case XCB_INPUT_BUTTON_RELEASE: {
            const bool is_press = gev->event_type == XCB_INPUT_BUTTON_PRESS;
            xcb_input_button_press_event_t* ev = (xcb_input_button_press_event_t*) gev;

            if(ev->detail == 4 || ev->detail == 5) {
                const int32_t dt = ev->detail == 4 ? 1 : -1;
                fn__notify_mouse_wheel_moved(
                    fp1616_to_int32(ev->event_x), fp1616_to_int32(ev->event_y),
                    dt
                );

                return;
            }

            enum fn_button button = 0;
            if(ev->detail == 1)
                button = fn_button_left;
            else if(ev->detail == 2)
                button = fn_button_middle;
            else if(ev->detail == 3)
                button = fn_button_right;

            fn__notify_button_changed(
                button,
                fp1616_to_int32(ev->event_x),
                fp1616_to_int32(ev->event_y),
                is_press
            );
        } break;
    }
}

void fn__imp_window_poll_events()
{
    xcb_generic_event_t* ev = NULL;
    struct fn_event fev = {0,};

    while((ev = xcb_poll_for_event(fn__g_window_context.connection))) {
        switch(ev->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE: {
                xcb_client_message_event_t* cev = (xcb_client_message_event_t*) ev;

                const uint32_t idx = get_fundament_id_from_window(cev->window);
                if(cev->data.data32[0] == fn__g_window_context.atom_delete_window)
                    fn__notify_window_destroyed(idx);

            } break;

            case XCB_CONFIGURE_NOTIFY: {
                xcb_configure_notify_event_t* cev = (xcb_configure_notify_event_t*) ev;

                const uint32_t idx = get_fundament_id_from_window(cev->window);
                struct fn__window* win = &fn__g_window_context.windows[idx];
                if(win->width == cev->width && win->height == cev->height)
                    break;

                fn__notify_window_resized(
                    idx, (uint32_t) win->width, (uint32_t) win->height
                );

            } break;

            case XCB_FOCUS_IN: {
                xcb_focus_in_event_t* cev = (xcb_focus_in_event_t*) ev;

                const uint32_t idx = get_fundament_id_from_window(cev->event);
                fn__notify_window_gained_focus(idx);
            } break;

            case XCB_FOCUS_OUT: {
                xcb_focus_out_event_t* cev = (xcb_focus_out_event_t*) ev;

                const uint32_t idx = get_fundament_id_from_window(cev->event);
                fn__notify_window_lost_focus(idx);
            }
                break;

            case XCB_MOTION_NOTIFY: {
                xcb_motion_notify_event_t* cev = (xcb_motion_notify_event_t*) ev;

                fn__notify_mouse_moved((uint32_t) cev->event_x,
                                       (uint32_t) cev->event_y
                );
            } break;

            case XCB_GE_GENERIC: {
                xcb_ge_generic_event_t* gev = (xcb_ge_generic_event_t*) ev;
                if(fn__g_window_context.has_xinput
                    && gev->extension == fn__g_window_context.opcode_xinput)
                    process_xinput_event(gev);
            } break;

            default:
                break;
        }

        free(ev);
    }
}
