#include <fundament/window.h>
#include <fundament/event.h>
#include <fundament/gl_context.h>

#include <stdio.h>

#if defined(__linux__)
    #include <GL/gl.h>
#elif defined(_WIN32)
    #include <Windows.h>
    #include <gl/gl.h>
#endif

int main() {
    fn_init_window_module();
    fn_init_gl_module();

    struct fn_window win = fn_create_window();
    fn_window_set_title(win, "GL Window");
    fn_window_set_width(win, 800);
    fn_window_set_height(win, 600);
    fn_window_set_visibility(win, true);

    struct fn_gl_context ctx = fn_create_gl_context(&(struct fn_gl_context_desc) {
        .major_version = 2,
        .minor_version = 0,
        .is_double_buffered = true
    });

    if(ctx.id == 0)
        printf("Failed to create the OpenGL context.\n");

    bool success = fn_gl_context_make_current(
        ctx,
        fn_window_handle(win)
    );

    fn_gl_context_set_vsync(true);

    if(fn_gl_context_extension_supported("GL_ARB_explicit_attrib_location"))
        printf("GL_ARB_explicit_attrib_location is supported!\n");

    if(!success)
        printf("Failed to make OpenGL context current.\n");

    glClearColor(0.4f, 0.6f, 0.9f, 0.0f);

    struct fn_event ev = {0, };
    while(ev.type != fn_event_type_closed) {
        glClear(GL_COLOR_BUFFER_BIT);
        fn_gl_context_present();
        fn_poll_events(&ev);
    }



    fn_deinit_window_module();
    return 0;
}