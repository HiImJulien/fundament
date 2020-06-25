#include "window_xcb.h"
#include "input_xcb.h"
#include "../input_common.h"
#include "../window_common.h"
#include <fundament/event.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#include <xcb/xcb.h>
#include <xcb/xinput.h>

static Display* display = NULL;
static xcb_connection_t* connection = NULL;
static xcb_screen_t* screen = NULL;
static xcb_atom_t atom_protocols = 0;
static xcb_atom_t atom_delete_window = 0;

// This atom is used to attach the window id,
// which the again is used to by the event handler
// to determine the exact window.
static xcb_atom_t atom_fundament_id = 0;

static bool xinput_present = false;
static uint8_t xinput_opcode = 0;
static uint16_t xinput_version = (
    ((uint8_t) XCB_INPUT_MAJOR_VERSION << 8)
    | (uint8_t) XCB_INPUT_MINOR_VERSION
);

static uint32_t get_fundament_id_from_window(fn_native_window_handle_t handle) {
    xcb_get_property_cookie_t id_cookie = xcb_get_property(
        connection,
        false,
        handle,
        atom_fundament_id,
        XCB_ATOM_INTEGER,
        0,
        1
    );

    xcb_get_property_reply_t* reply = xcb_get_property_reply(
        connection,
        id_cookie,
        NULL
    );

    xcb_flush(connection);

    uint32_t idx = *(uint32_t*) xcb_get_property_value(reply);
    xcb_flush(connection);
    free(reply); 

    return idx;
}

void fn__imp_init_window_context() {
    display = XOpenDisplay(0);
    connection = XGetXCBConnection(display);
    XSetEventQueueOwner(display, XCBOwnsEventQueue);

    screen = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    
    xcb_intern_atom_cookie_t protocols_cookie = xcb_intern_atom(
        connection,
        1,
        12,
        "WM_PROTOCOLS"
    );

    xcb_intern_atom_reply_t* protocols_reply = xcb_intern_atom_reply(
        connection,
        protocols_cookie,
        0
    );

    xcb_intern_atom_cookie_t delete_window_cookie = xcb_intern_atom(
        connection,
        1,
        16,
        "WM_DELETE_WINDOW"
    );

    xcb_intern_atom_reply_t* delete_window_reply = xcb_intern_atom_reply(
        connection,
        delete_window_cookie,
        0
    );

    xcb_intern_atom_cookie_t id_cookie = xcb_intern_atom(
        connection,
        0,
        12,
        "FUNDAMENT_ID" 
    );

    xcb_intern_atom_reply_t* id_cookie_reply = xcb_intern_atom_reply(
        connection,
        id_cookie,
        0
    );
    
    // Check whether XInput is supported.
    xcb_query_extension_cookie_t qxi_cookie = xcb_query_extension(
        connection,
        15,
        "XInputExtension"
    );

    xcb_query_extension_reply_t* qxi_reply = xcb_query_extension_reply(
        connection,
        qxi_cookie,
        NULL
    );

    xcb_input_get_extension_version_cookie_t xiv_cookie =
        xcb_input_get_extension_version(
            connection,
            15,
            "XInputExtension"
        );

    xcb_input_get_extension_version_reply_t* xiv_reply =
        xcb_input_get_extension_version_reply(
            connection,
            xiv_cookie,
            NULL
        );

    xcb_flush(connection);

    atom_protocols = protocols_reply->atom;
    atom_delete_window = delete_window_reply->atom;
    atom_fundament_id = id_cookie_reply->atom; 

    const uint16_t reported_version = 
        (xiv_reply->server_major << 8) | xiv_reply->server_minor;
     
    if(qxi_reply->present && xinput_version >= reported_version) {
        xinput_present = true;
        xinput_opcode = qxi_reply->major_opcode; 

        xcb_input_event_mask_t head = {
            .deviceid = XCB_INPUT_DEVICE_ALL,
            .mask_len = sizeof(xcb_input_xi_event_mask_t) / sizeof(uint32_t)
        };

        xcb_input_xi_select_events(
            connection,
            screen->root,
            1,
            &head
        );

        xcb_flush(connection);
    }

    free(protocols_reply);
    free(delete_window_reply);
    free(id_cookie_reply);
    free(qxi_reply);
}

fn_native_window_handle_t fn__imp_create_window(uint32_t index) {
    fn_native_window_handle_t handle = xcb_generate_id(connection); 
    
    const uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    const uint32_t mask_values[] = {
        screen->white_pixel,
        XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_BUTTON_PRESS
        | XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION
        | XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE
        | XCB_EVENT_MASK_STRUCTURE_NOTIFY
    };

    xcb_create_window(
        connection,
        XCB_COPY_FROM_PARENT,
        handle,
        screen->root,
        0,
        0,
        150,
        150,
        10,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        mask,
        mask_values
    );

    xcb_change_property(
        connection,
        XCB_PROP_MODE_REPLACE,
        handle,
        atom_protocols,
        4,
        32,
        1,
        &atom_delete_window
    );
   
    xcb_change_property(
        connection,
        XCB_PROP_MODE_REPLACE,
        handle,
        atom_fundament_id,
        XCB_ATOM_INTEGER,
        32,
        1,
        &index
    );

    xcb_map_window(connection, handle);
    xcb_flush(connection);

    if(xinput_present) {
        xcb_input_event_mask_t head = {
            .deviceid = XCB_INPUT_DEVICE_ALL,
            .mask_len = sizeof(xcb_input_xi_event_mask_t) / sizeof(uint32_t)
        };
       
        uint32_t* mask = xcb_input_event_mask_mask(&head);
        mask[0] = 
            XCB_INPUT_XI_EVENT_MASK_KEY_PRESS
            | XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE
            | XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS
            | XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE;

        xcb_input_xi_select_events(
            connection,
            handle,
            1,
            &head
        );

        xcb_flush(connection);    
    }

    return handle;
}

void fn__imp_destroy_window(fn_native_window_handle_t handle) {
    xcb_destroy_window(connection, handle);
    xcb_flush(connection);
}

void fn__imp_window_set_size(
    fn_native_window_handle_t handle,
    uint32_t width,
    uint32_t height
) {
    const uint32_t values[] = {
        width,
        height
    };

    xcb_configure_window(
        connection,
        handle,
        XCB_CONFIG_WINDOW_WIDTH
        | XCB_CONFIG_WINDOW_HEIGHT,
        values
    );

    xcb_flush(connection);
}

void fn__imp_window_set_title(
    fn_native_window_handle_t handle,
    const char* title) {
    // Sets the window's title
    xcb_change_property(
        connection,
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
        connection,
        XCB_PROP_MODE_REPLACE,
        handle,
        XCB_ATOM_WM_ICON_NAME,
        XCB_ATOM_STRING,
        8,
        strlen(title),
        title
    );
  
    xcb_flush(connection);
}

void fn__imp_window_set_visibility(
    fn_native_window_handle_t handle,
    bool visible) {

    // I am currently not satisfied with this 
    // solution, since an 'unmapped' window
    // doesn't allow many operations.
    // I believe drawing to may cause crashes,
    // altough I am not sure.

    if(visible)
        xcb_map_window(connection, handle);
    else
        xcb_unmap_window(connection, handle);

    xcb_flush(connection);
}

static void process_xinput_event(xcb_ge_generic_event_t* gev) {
    switch(gev->event_type) {
        case XCB_INPUT_KEY_PRESS: {
            xcb_input_device_key_press_event_t* ev = 
                (xcb_input_device_key_press_event_t*) gev;

            const char loc = fn__imp_translate_key(
                display, 
                (uint32_t) ev->child
            ); 

            // Values offset by 8 compared to linux' key codes?
            fn__imp_process_keyboard_input(
                (uint32_t) ev->child,
                true,
                loc
            );
        } break;

        case XCB_INPUT_KEY_RELEASE: {
            xcb_input_device_key_release_event_t* ev =
                (xcb_input_device_key_release_event_t*) gev;

            const char loc = fn__imp_translate_key(
                display, 
                (uint32_t) ev->child
            );

            fn__imp_process_keyboard_input(
                (uint32_t) ev->child,
                false,
                loc
            );
        } break;

        case XCB_INPUT_BUTTON_PRESS: 
        case XCB_INPUT_BUTTON_RELEASE: {
            const bool is_press = gev->event_type == XCB_INPUT_BUTTON_PRESS;
            xcb_input_button_press_event_t* ev =
                (xcb_input_button_press_event_t*) gev;

            struct fn_event fev = {0, };
            if(ev->detail == 4 || ev->detail == 5) {
                fev.type = fn_event_type_mouse_wheel;
                fev.mouse_wheel = ev->detail == 4 ? 1 : -1;
                fn__push_event(&fev); 
            }

            enum fn_button button = 0;
            if(ev->detail == 1)
                button = fn_button_left;
            else if(ev->detail == 2)
                button = fn_button_middle;
            else if(ev->detail == 3)
                button = fn_button_right;

            fn__set_button_state(button, true);

            fev.type = is_press
                ? fn_event_type_button_pressed
                : fn_event_type_button_released;

            fev.button = button;
            fev.x = (int32_t)((float) ev->event_x / (float)(1 << 16));
            fev.y = (int32_t)((float) ev->event_y / (float)(1 << 16));
            fn__push_event(&fev);
               
        } break;
    }
}

void fn__imp_window_poll_events() {
    xcb_generic_event_t* ev = NULL;
    struct fn_event fev = {0, };
    
    while((ev = xcb_poll_for_event(connection))) {
        switch(ev->response_type & ~0x80) {

            case XCB_CLIENT_MESSAGE: {
                xcb_client_message_event_t* cev =
                    (xcb_client_message_event_t*) ev;

                uint32_t idx = get_fundament_id_from_window(cev->window);

                if(cev->data.data32[0] == atom_delete_window) {
                    fev.type = fn_event_type_closed;
                    fev.window.id = idx + 1;
                    fn__push_event(&fev);
                }
            } break;

            case XCB_CONFIGURE_NOTIFY: {
                xcb_configure_notify_event_t* cev =
                    (xcb_configure_notify_event_t*) ev;

                uint32_t idx = get_fundament_id_from_window(cev->window);
                struct fn__window* win = &fn__g_window_context.windows[idx];

                if(win->width == cev->width && win->height == cev->height)
                    break;

                win->width = cev->width;
                win->height = cev->height;

                fev.type = fn_event_type_resized;
                fev.width = win->width;
                fev.height = win->height;

                fn__push_event(&fev); 
            } break;

            case XCB_FOCUS_IN: {
                xcb_focus_in_event_t* cev =
                    (xcb_focus_in_event_t*) ev;

                uint32_t idx = get_fundament_id_from_window(cev->event); 
                fev.type = fn_event_type_focus_gained;
                fev.window.id = idx + 1;
                fn__push_event(&fev);
            } break;

            case XCB_FOCUS_OUT: {
                xcb_focus_out_event_t* cev =
                    (xcb_focus_out_event_t*) ev;

                uint32_t idx = get_fundament_id_from_window(cev->event);
                fev.type = fn_event_type_focus_lost;
                fev.window.id = idx + 1;
                fn__push_event(&fev);
            } break;

            case XCB_KEY_PRESS: {
                xcb_key_press_event_t* cev = 
                    (xcb_key_press_event_t*) ev;

                uint32_t idx = get_fundament_id_from_window(cev->event);
                
                fev.type = fn_event_type_key_pressed;
                fev.window.id = idx + 1;
                fn__push_event(&fev);
            } break;

            case XCB_KEY_RELEASE: {
                xcb_key_release_event_t* cev =
                    (xcb_key_release_event_t*) ev;

                uint32_t idx = get_fundament_id_from_window(cev->event); 
                
                fev.type = fn_event_type_key_released;
                fev.window.id = idx + 1;
                fn__push_event(&fev);
            } break;

            case XCB_BUTTON_PRESS: {
                xcb_button_press_event_t* cev =
                    (xcb_button_press_event_t*) ev;

                printf("Ev: %"PRIu32"\n", cev->event);
                uint32_t idx = get_fundament_id_from_window(cev->event);

                if((cev->detail & XCB_BUTTON_MASK_4) != 0
                    || cev->detail & XCB_BUTTON_MASK_5) {
                    fev.type = fn_event_type_mouse_wheel;
                    fev.window.id = idx + 1;
                    fev.button = fn__get_pressed_buttons();
                    fev.x = cev->event_x;
                    fev.y = cev->event_y;
                    fn__push_event(&fev);
                    break;
                }

                if(cev->detail & XCB_BUTTON_MASK_1)
                    fn__set_button_state(fn_button_left, true);

                if(cev->detail & XCB_BUTTON_MASK_2)
                    fn__set_button_state(fn_button_right, true);

                if(cev->detail & XCB_BUTTON_MASK_3)
                    fn__set_button_state(fn_button_middle, true);
                    
                fev.type = fn_event_type_button_pressed;
                fev.window.id = idx + 1;
                fev.button = fn__get_pressed_buttons();
                fev.x = cev->event_x;
                fev.y = cev->event_y;
                fn__push_event(&fev);
            } break;

            case XCB_BUTTON_RELEASE: {
                xcb_button_release_event_t* cev = 
                    (xcb_button_release_event_t*) ev;

                uint32_t idx = get_fundament_id_from_window(cev->event);

                if(cev->event & XCB_BUTTON_MASK_1)
                    fn__set_button_state(fn_button_left, false); 

                if(cev->event & XCB_BUTTON_MASK_2)
                    fn__set_button_state(fn_button_right, false);
        
                if(cev->event & XCB_BUTTON_MASK_3)
                    fn__set_button_state(fn_button_middle, false);

                fev.type = fn_event_type_button_released;
                fev.window.id = idx + 1,
                fev.button = fn__get_pressed_buttons();
                fev.x = cev->event_x;
                fev.y = cev->event_y;
                fn__push_event(&fev);
            } break;

            case XCB_MOTION_NOTIFY: {
                xcb_motion_notify_event_t* cev =
                    (xcb_motion_notify_event_t*) ev;

                uint32_t idx = get_fundament_id_from_window(cev->event);

                fev.type = fn_event_type_mouse_moved;
                fev.window.id = idx + 1;
                fev.x = cev->event_x;
                fev.y = cev->event_y;
                fn__push_event(&fev);
            } break;
            
            case XCB_GE_GENERIC: {
                xcb_ge_generic_event_t* gev = (xcb_ge_generic_event_t*) ev;
                if(gev->extension == xinput_opcode)
                    process_xinput_event(gev);

            } break;
 
            default: break;
        }

        free(ev);
    }
}
