#include "input_xcb.h"
#include <fundament/event.h>
#include "../window_common.h"

#include <X11/Xutil.h>
#include <linux/input-event-codes.h>

char fn__imp_translate_key(Display* dpy, uint32_t keycode) {
    char buffer[16];

    // XLookupString requires an XKeyEvent,
    // here we create a dummy event.
    XKeyEvent ev= {
        0,
    };
    ev.display= dpy;
    ev.keycode= keycode;
    ev.state  = 0;

    // There is little documentation on using XInput with XCB.
    // So far I deduced, by simply trying out, that a state behaves like
    // this:
    // 1 -> Shift
    // 2 -> Tab
    // 3 -> Uhm, no fucking idea.

    if(fn__get_key_state(fn_key_left_shift) || 
        fn__get_key_state(fn_key_right_shift))
        ev.state= 1;
    else if(fn__get_key_state(fn_key_tab))
        ev.state= 2;

    const int res= XLookupString(&ev, buffer, 16, NULL, NULL);

    return res == 0 ? 0 : buffer[0];
}
