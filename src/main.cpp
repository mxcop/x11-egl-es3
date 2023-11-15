#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstdarg>

#include <X11/Xlib.h>
#include <glad/egl.h>

void error_fatal(const char* format, ...) {
    printf("error: ");

    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);

    printf("\n");
    exit(1);
}

const EGLint egl_config_attribs[] = {
    EGL_COLOR_BUFFER_TYPE,     EGL_RGB_BUFFER,
    EGL_BUFFER_SIZE,           32,
    EGL_RED_SIZE,              8,
    EGL_GREEN_SIZE,            8,
    EGL_BLUE_SIZE,             8,
    EGL_ALPHA_SIZE,            8,

    EGL_DEPTH_SIZE,            24,
    EGL_STENCIL_SIZE,          8,

    EGL_SAMPLE_BUFFERS,        0,
    EGL_SAMPLES,               0,

    EGL_SURFACE_TYPE,          EGL_WINDOW_BIT,
    EGL_RENDERABLE_TYPE,       EGL_OPENGL_ES2_BIT,

    EGL_NONE,
};

const EGLint egl_context_attribs[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE,
};

const EGLint egl_surface_attribs[] = {
    EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
    EGL_NONE,
};

void setup_egl(EGLint api, EGLNativeDisplayType native_display,
               EGLNativeWindowType native_window, EGLDisplay* out_display,
               EGLConfig* out_config, EGLContext* out_context,
               EGLSurface* out_window_surface) {
    EGLint ignore;
    EGLint major = 1, minor = 4;
    EGLBoolean ok;

    ok = eglBindAPI(api);
    if (!ok) error_fatal("eglBindAPI(0x%x) failed", api);

    EGLDisplay display = eglGetDisplay(native_display);
    if (display == EGL_NO_DISPLAY) error_fatal("eglGetDisplay() failed");
    
    ok = eglInitialize(display, &major, &minor);
    if (!ok) error_fatal("eglInitialize() failed");

    EGLint configs_size = 256;
    EGLConfig* configs = new EGLConfig[configs_size];
    EGLint num_configs;
    ok = eglChooseConfig(display, egl_config_attribs, configs,
                         configs_size,   // num requested configs
                         &num_configs);  // num returned configs
    if (!ok) error_fatal("eglChooseConfig() failed");
    if (num_configs == 0) error_fatal("failed to find suitable EGLConfig");
    EGLConfig config = configs[0];
    delete[] configs;

    EGLContext context =
        eglCreateContext(display, config, EGL_NO_CONTEXT, egl_context_attribs);
    if (!context) error_fatal("eglCreateContext() failed");

    EGLSurface surface = eglCreateWindowSurface(display, config, native_window,
                                                egl_surface_attribs);
    if (!surface) error_fatal("eglCreateWindowSurface() failed");

    ok = eglMakeCurrent(display, surface, surface, context);
    if (!ok) error_fatal("eglMakeCurrent() failed");

    // Check if surface is double buffered.
    EGLint render_buffer;
    ok = eglQueryContext(display, context, EGL_RENDER_BUFFER, &render_buffer);
    if (!ok) error_fatal("eglQueyContext(EGL_RENDER_BUFFER) failed");
    if (render_buffer == EGL_SINGLE_BUFFER)
        printf("warn: EGL surface is single buffered\n");

    *out_display = display;
    *out_config = config;
    *out_context = context;
    *out_window_surface = surface;
}

int main(void) {
    Display* x_display;
    Window x_window;
    XEvent e;
    const char* msg = "Hello, World!";
    int s;

    x_display = XOpenDisplay(NULL);
    if (x_display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    s = DefaultScreen(x_display);
    x_window = XCreateSimpleWindow(x_display, RootWindow(x_display, s), 10, 10, 100, 100, 1,
                            BlackPixel(x_display, s), WhitePixel(x_display, s));
    XSelectInput(x_display, x_window, ExposureMask | KeyPressMask);
    XMapWindow(x_display, x_window);

    EGLDisplay e_display = eglGetDisplay(x_display);

    // EGLDisplay egl_display;
    // EGLConfig egl_config;
    // EGLContext egl_context;
    // EGLSurface egl_surface;
    // setup_egl(EGL_OPENGL_ES_API,
    //           x_display,
    //           x_window,
    //           &egl_display,
    //           &egl_config,
    //           &egl_context,
    //           &egl_surface);

    while (1) {
        XNextEvent(x_display, &e);
        // if (e.type == Expose) {
        //     XFillRectangle(x_display, w, DefaultGC(d, s), 20, 20, 10, 10);
        //     XDrawString(d, w, DefaultGC(d, s), 10, 50, msg, strlen(msg));
        // }
        if (e.type == KeyPress) break;
    }

    XCloseDisplay(x_display);
    return 0;
}
