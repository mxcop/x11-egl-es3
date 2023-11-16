#include <stdio.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <GLES2/gl2.h>
#include <EGL/egl.h>

/* Set to '0' to disable v-sync */
#define VSYNC 1

/**
 * @brief Initialize an X11 window.
 */
Window init_x11_window(Display* display, int x, int y, int w, int h,
                       const char* title) {
    /* Get root window (usually whole screen) */
    Window root_win = DefaultRootWindow(display);

    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | ButtonPressMask | KeyPressMask;

    /* Create our window */
    Window x_win =
        XCreateWindow(display, root_win, 0, 0, 800, 600, 0, CopyFromParent,
                      InputOutput, CopyFromParent, CWEventMask, &swa);

    /* Update window title */
    XStoreName(display, x_win, title);

    /* Window hints */
    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(display, x_win, &hints);

    /* Show the window */
    XMapWindow(display, x_win);
    return x_win;
}

/**
 * @brief Properly cleanup X11 resources.
 */
void cleanup_x11(Display* d, Window& win) {
    XDestroyWindow(d, win);
    XCloseDisplay(d);
}

/**
 * @brief (Attempt) to initialize the EGL context.
 */
int init_egl_ctx(Display* display, Window& window, EGLDisplay& out_egl_display,
                 EGLSurface& out_egl_surface, EGLContext& out_egl_context) {
    /* Shorter names */
    EGLDisplay& egl_display = out_egl_display;
    EGLSurface& egl_surface = out_egl_surface;
    EGLContext& egl_context = out_egl_context;

    egl_display = eglGetDisplay((EGLNativeDisplayType)display);
    if (egl_display == EGL_NO_DISPLAY) {
        printf("Got no EGL display.\n");
        return 1;
    }

    EGLint egl_version_major, egl_version_minor;
    if (!eglInitialize(egl_display, &egl_version_major, &egl_version_minor)) {
        printf("Unable to initialize EGL.\n");
        return 1;
    }
    printf("Initialized EGL version %d.%d\n", egl_version_major,
           egl_version_minor);

    EGLint egl_config_constraints[] = {EGL_RED_SIZE,
                                       8,
                                       EGL_GREEN_SIZE,
                                       8,
                                       EGL_BLUE_SIZE,
                                       8,
                                       EGL_ALPHA_SIZE,
                                       0,
                                       EGL_RENDERABLE_TYPE,
                                       EGL_OPENGL_ES3_BIT,
                                       EGL_CONFIG_CAVEAT,
                                       EGL_NONE,
                                       EGL_NONE};

    EGLConfig egl_conf;
    EGLint num_config;
    if (!eglChooseConfig(egl_display, egl_config_constraints, &egl_conf, 1,
                         &num_config)) {
        printf("Failed to choose config. (%d)\n", eglGetError());
        return 1;
    }

    if (num_config != 1) {
        printf("Got %d configs...\n", num_config);
        return 1;
    }

    egl_surface = eglCreateWindowSurface(egl_display, egl_conf, window, NULL);
    if (egl_surface == EGL_NO_SURFACE) {
        printf("Unable to create EGL surface. (%d)\n", eglGetError());
        return 1;
    }

    EGLint ctxattr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    egl_context =
        eglCreateContext(egl_display, egl_conf, EGL_NO_CONTEXT, ctxattr);
    if (egl_context == EGL_NO_CONTEXT) {
        printf("Unable to create EGL context. (%d)\n", eglGetError());
        return 1;
    }

    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

    /* Print some info */
#if DEBUG
    printf("EGL > Client APIs: %s\n",
           eglQueryString(egl_display, EGL_CLIENT_APIS));
    printf("EGL >      Vendor: %s\n", eglQueryString(egl_display, EGL_VENDOR));
    printf("EGL >     Version: %s\n", eglQueryString(egl_display, EGL_VERSION));
    // printf("EGL - Extensions: \n%s\n", eglQueryString(egl_display, EGL_EXTENSIONS));
#endif

    {
        EGLint queried_buffer;
        if (!eglQueryContext(egl_display, egl_context, EGL_RENDER_BUFFER,
                             &queried_buffer)) {
            printf("Warn: Failed to query EGL_RENDER_BUFFER: %d\n",
                   eglGetError());
        }
    }

#if VSYNC
    /* V-sync */
    if (!eglSwapInterval(egl_display, 1)) {
        printf("Warn: eglSwapInterval failed: %d\n", eglGetError());
    }
#endif
    return 0;
}

/**
 * @brief Properly cleanup EGL resources.
 */
void cleanup_egl(EGLDisplay& d, EGLContext& ctx, EGLSurface& srf) {
    eglMakeCurrent(d, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(d, ctx);
    eglDestroySurface(d, srf);
    eglTerminate(d);
}

int main(int argc, char** argv) {
    /* Get X11 display */
    Display* x_display = XOpenDisplay(NULL);
    if (x_display == NULL) {
        printf("Cannot connect to X server.\n");
        return 1;
    }

    Window x_win = init_x11_window(x_display, 0, 0, 800, 600, "Hello, world!");

    /* Register interest in window close events */
    Atom wm_delmsg = XInternAtom(x_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(x_display, x_win, &wm_delmsg, 1);

    /* Initialize the EGL context */
    EGLDisplay egl_display;
    EGLSurface egl_surface;
    EGLContext egl_context;
    if (init_egl_ctx(x_display, x_win, egl_display, egl_surface, egl_context)) {
        printf("Error creating EGL context.\n");
        return 1;
    }

    { /* Set viewport dimension */
        XWindowAttributes gwa;
        XGetWindowAttributes(x_display, x_win, &gwa);
        glViewport(0, 0, gwa.width, gwa.height);
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    /* Program loop */
    for (XEvent x_ev;;) {
        XNextEvent(x_display, &x_ev);

        /* Exit loop if window is closed */
        if (x_ev.type == ClientMessage &&
            x_ev.xclient.data.l[0] == wm_delmsg) {
            printf("Window was closed.\n");
            break;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        /* Do stuff here :) */

        eglSwapBuffers(egl_display, egl_surface);
    }

    /* Cleanup */
    cleanup_egl(egl_display, egl_context, egl_surface);
    cleanup_x11(x_display, x_win);

    return 0;
}
