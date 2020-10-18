#include <fundament/window.h>
#include <fundament/event.h>
#include <fundament/gl_context.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
    #include <Windows.h>
    #include <gl/GL.h>
#endif 

int main(int argc, char** argv) {
    fn_init_window();
    fn_init_opengl();
    printf("Initialized modules.\n");

    struct fn_window window = fn_create_window();
    if (window.id == 0)
        printf("Failed to create window.\n");

    printf("Window id: %"PRIu32"\n", window.id);

    fn_window_set_size(window, 800, 600);
    fn_window_set_title(window, "fundament: Window Sample");
    fn_window_set_visible(window, true);
    printf("Created window.\n");

    struct fn_gl_context ctx = fn_create_gl_context(&(struct fn_gl_context_desc) {
        .major_version = 4,
        .minor_version = 0,
        .is_double_buffered = true
    });
    
    if (!fn_gl_context_make_current(ctx, fn_window_handle(window)))
        printf("Failed to make context current.\n");

    glClearColor(0.4f, 0.6f, 0.9f, 0.0f);

    printf("GL context id: %"PRIu32"\n", ctx.id);

    struct fn_event ev = { 0, };
    while (ev.type != fn_event_type_closed) {
        glClear(GL_COLOR_BUFFER_BIT);
        
        fn_gl_context_present();
        fn_poll_events(&ev);
    }

    printf("Deinitializing modules.\n");
    fn_deinit_opengl();
    fn_deinit_window();
    return EXIT_SUCCESS;
}