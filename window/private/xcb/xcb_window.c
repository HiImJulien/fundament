#include "xcb_window.h"
#include "xcb_window_map.h"
#include "../window_common.h"

#include <stdlib.h>
#include <string.h>

#include <xcb/xcb.h>

//==============================================================================
//                             INTERNAL FUNCTIONS
//==============================================================================

static xcb_connection_t* fn__g_connection = NULL;
static xcb_screen_t* fn__g_screen = NULL;
static xcb_atom_t fn__g_atom_protocols = 0;
static xcb_atom_t fn__g_atom_del_window = 0;

static void fn__xcb_on_client_message(xcb_generic_event_t* gev) {
    xcb_client_message_event_t* ev = (xcb_client_message_event_t*) gev;

    if(ev->data.data32[0] != fn__g_atom_del_window)
        return;

    struct fn__window* ptr = fn__xcb_window_map_get(ev->window);
    if(ptr) {
        fn__notify_window_closed(ptr);
        fn__xcb_window_map_remove(ptr);
    }
}

static void fn__xcb_configure_notify(xcb_generic_event_t* gev) {
    xcb_configure_notify_event_t* ev = (xcb_configure_notify_event_t*) gev;

    struct fn__window* ptr = fn__xcb_window_map_get(ev->window);
    if(!ptr)
        return;

    if(ptr->width == ev->width && ptr->height == ev->height)
        return;

    fn__notify_window_resized(
        ptr,
        ev->width,
        ev->height
    );
}

static void fn__xcb_focus_in(xcb_generic_event_t* gev) {
    xcb_focus_in_event_t* ev = (xcb_focus_in_event_t*) gev;
    struct fn__window* ptr = fn__xcb_window_map_get(ev->event);
    if(ptr)
        fn__notify_window_gained_focus(ptr);
}

static void fn__xcb_focus_out(xcb_generic_event_t* gev) {
    xcb_focus_out_event_t* ev = (xcb_focus_out_event_t*) gev;
    struct fn__window* ptr = fn__xcb_window_map_get(ev->event);
    if(ptr)
        fn__notify_window_lost_focus(ptr);
}

//==============================================================================
//               PUBLIC FUNCTIONS DECLARED IN "./xcb_window.h"
//==============================================================================

bool fn__init_xcb_window() {
    fn__g_connection = xcb_connect(NULL, NULL);
    if(fn__g_connection == NULL)
        return false;

    fn__g_screen = xcb_setup_roots_iterator(
        xcb_get_setup(fn__g_connection)
    ).data; 

    // Query atoms
    xcb_intern_atom_cookie_t protocols = xcb_intern_atom(
        fn__g_connection, 1, 12, "WM_PROTOCOLS"
    );

    xcb_intern_atom_reply_t* protocols_rp = xcb_intern_atom_reply(
        fn__g_connection, protocols, NULL
    );

    xcb_intern_atom_cookie_t del_window = xcb_intern_atom(
        fn__g_connection, 1, 16, "WM_DELETE_WINDOW"
    );

    xcb_intern_atom_reply_t* del_window_rp = xcb_intern_atom_reply(
        fn__g_connection, del_window, NULL
    );

    xcb_flush(fn__g_connection);

    fn__g_atom_protocols = protocols_rp->atom;
    fn__g_atom_del_window = del_window_rp->atom;

    free(protocols_rp);
    free(del_window_rp);

    return true;
}

void fn__deinit_xcb_window() {
    if(fn__g_connection) {
        xcb_disconnect(fn__g_connection);
        fn__g_connection = NULL;
    }
}

bool fn__create_xcb_window(struct fn__window* window) {
    window->native = xcb_generate_id(fn__g_connection);

    const uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t mask_values[] = {
        0,
        XCB_EVENT_MASK_EXPOSURE
        | XCB_EVENT_MASK_STRUCTURE_NOTIFY
        | XCB_EVENT_MASK_FOCUS_CHANGE
    };

    xcb_create_window(
        fn__g_connection,
        XCB_COPY_FROM_PARENT,
        window->native,
        fn__g_screen->root,
        0,
        0,
        150,
        150,
        10,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        fn__g_screen->root_visual,
        mask,
        mask_values
    );

    xcb_map_window(fn__g_connection, window->native);
    xcb_flush(fn__g_connection);

    xcb_change_property(
        fn__g_connection,
        XCB_PROP_MODE_REPLACE,
        window->native,
        fn__g_atom_protocols,
        4,
        32,
        1,
        &fn__g_atom_del_window
    );

    xcb_flush(fn__g_connection);
    fn__xcb_window_map_insert(window);

    return true;
}

void fn__destroy_xcb_window(struct fn__window* window) {
    xcb_destroy_window(
        fn__g_connection,
        window->native
    );

    xcb_flush(fn__g_connection); 
    window->native = FN_NULL_WINDOW;
}

void fn__xcb_window_set_size(
    struct fn__window* window,
    uint32_t width,
    uint32_t height
) {
    const uint32_t values[] = {width, height};

    xcb_configure_window(
        fn__g_connection,
        window->native,
        XCB_CONFIG_WINDOW_WIDTH | XCB_CONFIG_WINDOW_HEIGHT,
        values
    );

    xcb_flush(fn__g_connection);
}

void fn__xcb_window_set_title(
    struct fn__window* window,
    const char* title
) {
    xcb_change_property(
        fn__g_connection,
        XCB_PROP_MODE_REPLACE,
        window->native,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        strlen(title),
        title
    );

    xcb_change_property(
        fn__g_connection,
        XCB_PROP_MODE_REPLACE,
        window->native,
        XCB_ATOM_WM_ICON_NAME,
        XCB_ATOM_STRING,
        8,
        strlen(title),
        title
    );
}

void fn__xcb_window_set_visible(
    struct fn__window* window,
    bool visible
) {
    if(visible) xcb_map_window(fn__g_connection, window->native);
    else xcb_unmap_window(fn__g_connection, window->native);

    xcb_flush(fn__g_connection);
}

void fn__xcb_pump_events() {
    xcb_generic_event_t* ev = NULL;
    while((ev = xcb_poll_for_event(fn__g_connection))) {
        switch(ev->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE: fn__xcb_on_client_message(ev); break;
            case XCB_CONFIGURE_NOTIFY: fn__xcb_configure_notify(ev); break;
            case XCB_FOCUS_IN: fn__xcb_focus_in(ev); break;
            case XCB_FOCUS_OUT: fn__xcb_focus_out(ev); break;
                
            default:    break;
        }
        free(ev);
    }
}
