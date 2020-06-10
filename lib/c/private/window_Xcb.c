#include "window_Xcb.h"
#include "window_common.h"
#include <fundament/event.h> 

#include <stdlib.h>
#include <string.h>
#include <xcb/xcb.h>

static xcb_connection_t* connection = NULL;
static xcb_screen_t* screen = NULL;
static xcb_atom_t atom_protocols = 0;
static xcb_atom_t atom_delete_window = 0;

void fn__imp_init_window_context() {
    connection = xcb_connect(NULL, NULL);
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

    xcb_flush(connection);

    atom_protocols = protocols_reply->atom;
    atom_delete_window = delete_window_reply->atom;
    
    free(protocols_reply);
    free(delete_window_reply);
}

fn_native_window_handle_t fn__imp_create_window(uint32_t index) {
    fn_native_window_handle_t handle = xcb_generate_id(connection); 
    
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
        0,
        NULL
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
    
    xcb_map_window(connection, handle);

    xcb_flush(connection);

    return handle;
}

void fn__imp_destroy_window(fn_native_window_handle_t handle) {
    xcb_destroy_window(handle);
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


void fn__imp_window_poll_events() {
    xcb_generic_event_t* ev = NULL;
    struct fn_event fev = {0, };
    
    while((ev = xcb_poll_for_event(connection))) {
        switch(ev->response_type & ~0x80) {
            case XCB_CLIENT_MESSAGE: {
                xcb_client_message_event_t* cev =
                    (xcb_client_message_event_t*) ev;

                if(cev->data.data32[0] == atom_delete_window) {
                    fev.type = fn_event_type_closed;
                    fn__push_event(&fev);
                }
                    
            }
            default: break;
        }

        free(ev);
    }
}
