#ifndef FUNDAMENT_EVENT_H
#define FUNDAMENT_EVENT_H

//
// Enumerates known event types.
//
enum fn_event_type {
    fn_event_type_none,
    fn_event_type_closed,
    fn_event_type_resized,
};

//
// Event issued by the underlying system.
//
struct fn_event {
    enum fn_event_type  type;
};

#endif  // FUNDAMENT_EVENT_H