#ifndef FUNDAMENT_EVENT_H
#define FUNDAMENT_EVENT_H

#include <fundament/window.h>

#include <stdint.h>

//
// Enumerates known event types.
//
enum fn_event_type {
    fn_event_type_none,
    fn_event_type_closed,
    fn_event_type_resized,
    fn_event_type_focus_gained,
    fn_event_type_focus_lost,
};

//
// Event issued by the underlying system.
//
struct fn_event {
    enum fn_event_type  type;
    struct fn_window    window;

    struct {
        uint32_t    width;
        uint32_t    height;
    };
};

#endif  // FUNDAMENT_EVENT_H