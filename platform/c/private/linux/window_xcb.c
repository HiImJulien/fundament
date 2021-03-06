#include "window_xcb.h"
#include "../input_common.h"
#include "../window_common.h"
#include "input_key_map_xcb.h"

#include <stdlib.h>
#include <string.h>

#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <xcb/xcb.h>
#include <xcb/xinput.h>

//==============================================================================
// The following section defines utility functions used in the implementation.
//==============================================================================

struct fn__imp_map_entry {
    xcb_window_t    window;
    uint32_t        index;
};

static int fn__imp_map_entry_comparator(const void* vlhs, const void* vrhs) {
    const struct fn__imp_map_entry* lhs = vlhs;
    const struct fn__imp_map_entry* rhs = vrhs;

    if(lhs->window < rhs->window)
        return -1;
    else if(lhs->window > rhs->window)
        return 1;
    else
        return 0;
}

static size_t fn__imp_entry_index_at(xcb_window_t window, bool* found) {
    int32_t left = 0;
    int32_t right = fn__g_window_context.map_entries_size - 1;
    *found = false;

    if(fn__g_window_context.map_entries_size == 0)
        return 0;

    while(left <= right) {
        const size_t mid = left + (right - left) / 2;
        if(fn__g_window_context.map_entries[mid].window == window) {
            *found = true;
            return mid;
        } else if(fn__g_window_context.map_entries[mid].window < window)
            left = mid + 1;
        else
            right = mid - 1;
    }

    return 0;
}

static uint32_t fn__imp_entry_at(xcb_window_t window, bool* found) {
    const size_t index = fn__imp_entry_index_at(window, found);
    return fn__g_window_context.map_entries[index].index;
}

static void fn__imp_insert_entry(struct fn__imp_map_entry entry) {
    const size_t index = fn__g_window_context.map_entries_size++;
    fn__g_window_context.map_entries[index] = entry;

    qsort(
        fn__g_window_context.map_entries,
        fn__g_window_context.map_entries_size,
        sizeof(struct fn__imp_map_entry),
        fn__imp_map_entry_comparator
    );
}

static void fn__imp_remove_entry(xcb_window_t window) {
    bool found;
    const size_t index = fn__imp_entry_index_at(window, &found); 

    if(!found)
        return;

    for(size_t it = index; it < fn__g_window_context.map_entries_size - 1; ++it) {
        struct fn__imp_map_entry* dest = &fn__g_window_context.map_entries[it];   
        struct fn__imp_map_entry* src = &fn__g_window_context.map_entries[it + 1];
        *dest = *src;
    } 

    fn__g_window_context.map_entries_size--; 
}

//
// Returns the letter, that represents the given keycode.
//
static char fn__imp_translate_key(uint32_t keycode) {
    char buffer[16];

    XKeyEvent ev = {0, };
    ev.display = fn__g_window_context.display;
    ev.keycode = keycode;
    ev.state = 0;

    if(fn__get_key_state(fn_key_left_shift) 
        || fn__get_key_state(fn_key_right_shift))
        ev.state |= 1;
    else if(fn__get_key_state(fn_key_caps))
        ev.state |= 2;

    const int res = XLookupString(&ev, buffer, 16, NULL, NULL); 
    return res ? buffer[0] : 0;
}

//
// Processes events of type 'XCB_CLIENT_MESSAGE'.
//
static void fn__imp_on_client_message(xcb_generic_event_t* gev) {
    xcb_client_message_event_t* ev = (xcb_client_message_event_t*) gev; 

    if(ev->data.data32[0] == fn__g_window_context.atom_delete_window) {
        bool found;
        const uint32_t idx = fn__imp_entry_index_at(ev->window, &found);
        if(found) fn__notify_window_destroyed(idx);
        fn__imp_remove_entry(ev->window); 

        xcb_destroy_window(
            fn__g_window_context.connection,
            ev->window
        );

        xcb_flush(fn__g_window_context.connection);
    }
}

//
// Processes events of type 'XCB_CONFIGURE_NOTIFY'.
//
static void fn__imp_on_configure_notify(xcb_generic_event_t* gev) {
    xcb_configure_notify_event_t* ev = (xcb_configure_notify_event_t*) gev;

    bool found;
    const uint32_t idx = fn__imp_entry_at(ev->window, &found);
    if(!found)
        return;

    struct fn__window* win = &fn__g_window_context.windows[idx];

    if(win->width == ev->width && win->height == ev->height)
        return;

    fn__notify_window_resized(
        idx, 
        (uint32_t) ev->width,
        (uint32_t) ev->height
    );
}

//
// Processes events of type 'XCB_FOCUS_IN'.
//
static void fn__imp_on_focus_in(xcb_generic_event_t* gev) {
    xcb_focus_in_event_t* ev = (xcb_focus_in_event_t*) gev;

    bool found;
    const uint32_t idx = fn__imp_entry_at(ev->event, &found);
    if(found)
        fn__notify_window_gained_focus(idx);
}

//
// Processes events of type 'XCB_FOCUS_OUT'.
//
static void fn__imp_on_focus_out(xcb_generic_event_t* gev) {
    xcb_focus_out_event_t* ev = (xcb_focus_out_event_t*) gev;

    bool found;
    const uint32_t idx = fn__imp_entry_at(ev->event, &found);
    if(found) 
        fn__notify_window_lost_focus(idx);
}

//
// Processes events of type 'XCB_MOTION_NOTIFY'.
//
static void fn__imp_on_motion_notify(xcb_generic_event_t* gev) {
    xcb_motion_notify_event_t* ev = (xcb_motion_notify_event_t*) gev;

    fn__notify_mouse_moved(
        (uint32_t) ev->event_x,
        (uint32_t) ev->event_y
    );
}

//
// Converts fp1616 values to int32_t values.
//
static inline int32_t fn__imp_fp1616_to_int32(xcb_input_fp1616_t val) {
    return (int32_t) ((float) val / (float) (1 << 16));
}

//
// Processes XInput events.
//
static void fn__imp_on_xinput(xcb_ge_generic_event_t* gev) {
    switch(gev->event_type) {
        case XCB_INPUT_KEY_PRESS:
        case XCB_INPUT_KEY_RELEASE: {
            const bool press = gev->event_type == XCB_INPUT_KEY_PRESS;
            xcb_input_device_key_press_event_t* ev =
                (xcb_input_device_key_press_event_t*) gev;

            // The values of ev->child seem to be offset by 8 compared to the 
            // linux input event codes.
            const enum fn_key key = fn__imp_map_xcb_key((uint32_t) ev->child - 8); 
            const char letter = fn__imp_translate_key((uint32_t) ev->child);
            
            fn__notify_key_changed(key, letter, press); 
        } break;

        case XCB_INPUT_BUTTON_PRESS:
        case XCB_INPUT_BUTTON_RELEASE: {
            const bool press = gev->event_type == XCB_INPUT_BUTTON_PRESS;
            xcb_input_button_press_event_t* ev = 
                (xcb_input_button_press_event_t*) gev;

            const int32_t x = fn__imp_fp1616_to_int32(ev->event_x);
            const int32_t y = fn__imp_fp1616_to_int32(ev->event_y);

            if(ev->detail == 4 || ev->detail == 5) {
                const int32_t dt = ev->detail == 4 ? 1 : -1;
                fn__notify_mouse_wheel_moved(
                    x,
                    y,
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
                x,
                y,
                press
            );
        } break;

        case XCB_INPUT_MOTION: {
            xcb_input_motion_event_t* ev = 
                (xcb_input_motion_event_t*) gev;

            const int32_t x = fn__imp_fp1616_to_int32(ev->event_x);
            const int32_t y = fn__imp_fp1616_to_int32(ev->event_y); 

            fn__notify_mouse_moved(x, y);
        }
    }
}

//
// Proceeses events of type 'XCB_GE_GENERIC'.
//
static void fn__imp_on_generic(xcb_generic_event_t* gev) {
    xcb_ge_generic_event_t* ev = (xcb_ge_generic_event_t*) gev;

   if(fn__g_window_context.has_xinput 
      && ev->extension == fn__g_window_context.opcode_xinput) 
        fn__imp_on_xinput(ev); 
}

//==============================================================================
// The following section implements the window functions.
//============================================================================== 

void fn__imp_init_window_context()
{
    //--- Setup connection to server.
    Display* dpy = XOpenDisplay(NULL);
    fn__g_window_context.display = dpy; 
    fn__g_window_context.connection = XGetXCBConnection(dpy); 

    XSetEventQueueOwner(dpy, XCBOwnsEventQueue);

    fn__g_window_context.screen = xcb_setup_roots_iterator(
        xcb_get_setup(fn__g_window_context.connection)
    ).data;

    //--- Initializes the id_map.
    fn__g_window_context.map_entries_size = 0;
    fn__g_window_context.map_entries = malloc(
        sizeof(struct fn__imp_map_entry)
        * fn__g_window_context.windows_capacity
    );

    memset(
        fn__g_window_context.map_entries, 
        0, 
        sizeof(struct fn__imp_map_entry) * fn__g_window_context.windows_capacity
    );

    //--- Check wether XInput is supported by the server.
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

    if(qxi_reply->present) {
        fn__g_window_context.has_xinput = true;
        fn__g_window_context.opcode_xinput = qxi_reply->major_opcode;
    } else
        fn__g_window_context.has_xinput = false;

    free(qxi_reply);
    free(xiv_reply);

    //--- Retrieve extended window manager hint atoms.
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

    xcb_flush(fn__g_window_context.connection);

    fn__g_window_context.atom_protocols = protocols_reply->atom;
    fn__g_window_context.atom_delete_window = delete_window_reply->atom;

    free(protocols_reply);
    free(delete_window_reply);
}

void fn__imp_deinit_window_context() {
    free(fn__g_window_context.map_entries);
    fn__g_window_context.map_entries_size = 0;
    XCloseDisplay(fn__g_window_context.display); 
}

fn_native_window_handle_t fn__imp_create_window(uint32_t index)
{
    fn_native_window_handle_t handle = xcb_generate_id(
        fn__g_window_context.connection
    );

    const uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t mask_values[] = {
        fn__g_window_context.screen->white_pixel,
        XCB_EVENT_MASK_EXPOSURE 
        | XCB_EVENT_MASK_STRUCTURE_NOTIFY
        | XCB_EVENT_MASK_FOCUS_CHANGE
    };

    const uint32_t input_mask = XCB_EVENT_MASK_BUTTON_PRESS
        | XCB_EVENT_MASK_BUTTON_RELEASE
        | XCB_EVENT_MASK_POINTER_MOTION
        | XCB_EVENT_MASK_KEY_PRESS
        | XCB_EVENT_MASK_KEY_RELEASE
    ;

    if(!fn__g_window_context.has_xinput)
        mask_values[1] |= input_mask;

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

    xcb_map_window(fn__g_window_context.connection, handle);
    xcb_flush(fn__g_window_context.connection);

    if(fn__g_window_context.has_xinput) {
        xcb_input_event_mask_t head = {
            .deviceid= XCB_INPUT_DEVICE_ALL_MASTER,
            .mask_len= sizeof(xcb_input_xi_event_mask_t) / sizeof(uint32_t)
        };

        uint32_t* mask = xcb_input_event_mask_mask(&head);
        mask[0] = XCB_INPUT_XI_EVENT_MASK_KEY_PRESS 
                    | XCB_INPUT_XI_EVENT_MASK_KEY_RELEASE 
                    | XCB_INPUT_XI_EVENT_MASK_BUTTON_PRESS
                    | XCB_INPUT_XI_EVENT_MASK_BUTTON_RELEASE
                    | XCB_INPUT_XI_EVENT_MASK_MOTION; 

        xcb_input_xi_select_events(
            fn__g_window_context.connection, 
            handle, 
            1, 
            &head
        );

        xcb_flush(fn__g_window_context.connection);
    }

    fn__imp_insert_entry((struct fn__imp_map_entry) {
        .window = handle,
        .index = index 
    });

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

void fn__imp_window_poll_events()
{
    xcb_generic_event_t* ev = NULL;

    while((ev = xcb_poll_for_event(fn__g_window_context.connection))) {
        switch(ev->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE:
                fn__imp_on_client_message(ev);
                break;

            case XCB_CONFIGURE_NOTIFY:
                fn__imp_on_configure_notify(ev);
                break;

            case XCB_FOCUS_IN:
                fn__imp_on_focus_in(ev);
                break;
            
            case XCB_FOCUS_OUT:
                fn__imp_on_focus_out(ev);
                break;

            case XCB_MOTION_NOTIFY:
                fn__imp_on_motion_notify(ev);
                break;

            case XCB_GE_GENERIC:
                fn__imp_on_generic(ev);
                break;

            default:
                break;
        }

        free(ev);
    }
}
