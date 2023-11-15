#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include <glad/gl.h>
#include <glad/egl.h>
#include <string.h>

constexpr int WIN_X = 0, WIN_Y = 0, WIN_W = 800, WIN_H = 600;

int main(int argc, char **argv) {
    printf("Hello, world!\n");

    Display *x_display = XOpenDisplay(NULL);
    if (x_display == NULL) {
        printf("cannot connect to X server");
        return 1;
    }

    Window root = DefaultRootWindow(
        x_display);  // get the root window (usually the whole screen)
    int root_x, root_y;
    unsigned int root_w, root_h, root_border_width, root_depth;
    Window root_again;
    XGetGeometry(x_display, root, &root_again, &root_x, &root_y, &root_w,
                 &root_h, &root_border_width, &root_depth);
    printf("Matching X11 root window geometry: +%d,%d %dx%d border %d, %dbpp\n",
           root_x, root_y, root_w, root_h, root_border_width, root_depth);

    XSetWindowAttributes swa;
    swa.event_mask = ExposureMask | ButtonPressMask | KeyPressMask;

    Window win =
        XCreateWindow(x_display, root, 0, 0, root_w, root_h, 0, CopyFromParent,
                      InputOutput, CopyFromParent, CWEventMask, &swa);
    printf("Created window\n");

    XSetWindowAttributes xattr;

    xattr.override_redirect = False;
    XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

    /* This fails when there is no window manager running
    Atom atom;
    atom = XInternAtom(x_display, "_NET_WM_STATE_FULLSCREEN", True);
    XChangeProperty(
       x_display, win,
       XInternAtom(x_display, "_NET_WM_STATE", True),
       XA_ATOM, 32, PropModeReplace,
       (unsigned char*) &atom, 1);
    */

    int one = 1;
    XChangeProperty(
        x_display, win,
        XInternAtom(x_display, "_HILDON_NON_COMPOSITED_WINDOW", False),
        XA_INTEGER, 32, PropModeReplace, (unsigned char *)&one, 1);

    XWMHints hints;
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(x_display, win, &hints);

    XMapWindow(x_display, win);  // makes the window visible on the screen
    XStoreName(x_display, win, argv[0]);

    //// get identifiers for the provided atom name strings
    Atom wm_state = XInternAtom(x_display, "_NET_WM_STATE", False);
    Atom fullscreen = XInternAtom(x_display, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent xev;
    memset(&xev, 0, sizeof(xev));

    xev.type = ClientMessage;
    xev.xclient.window = win;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = 1;
    xev.xclient.data.l[1] = fullscreen;
    XSendEvent(  // set up event mask (which events we want to receive)
        x_display, DefaultRootWindow(x_display), False, SubstructureNotifyMask,
        &xev);
    printf("Done setting up window\n");

    ///////  the egl part
    /////////////////////////////////////////////////////////////////////
    //  egl provides an interface to connect the graphics related functionality
    //  of openGL ES with the windowing interface and functionality of the
    //  native operation system (X11 in our case.)

    EGLDisplay egl_display = eglGetDisplay((EGLNativeDisplayType)x_display);
    if (egl_display == EGL_NO_DISPLAY) {
        printf("Got no EGL display.");
        return 1;
    }

    EGLint egl_version_major, egl_version_minor;
    if (!eglInitialize(egl_display, &egl_version_major, &egl_version_minor)) {
        printf("Unable to initialize EGL");
        return 1;
    }
    printf("Initialized EGL version %d.%d", egl_version_major,
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
                                       EGL_OPENGL_ES2_BIT,
                                       EGL_CONFIG_CAVEAT,
                                       EGL_NONE,
                                       EGL_NONE};

    EGLConfig egl_conf;
    EGLint num_config;
    if (!eglChooseConfig(egl_display, egl_config_constraints, &egl_conf, 1,
                         &num_config)) {
        printf("Failed to choose config (eglError: %s)", eglGetError());
        return 1;
    }

    if (num_config != 1) {
        printf("Didn't get exactly one config, but %d", num_config);
        return 1;
    }

    EGLSurface egl_surface =
        eglCreateWindowSurface(egl_display, egl_conf, win, NULL);
    if (egl_surface == EGL_NO_SURFACE) {
        printf("Unable to create EGL surface (eglError: %s)", eglGetError());
        return 1;
    }

    //// egl-contexts collect all state descriptions needed required for
    /// operation
    EGLint ctxattr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    EGLContext egl_context =
        eglCreateContext(egl_display, egl_conf, EGL_NO_CONTEXT, ctxattr);
    if (egl_context == EGL_NO_CONTEXT) {
        printf("Unable to create EGL context (eglError: %s)", eglGetError());
        return 1;
    }

    //// associate the egl-context with the egl-surface
    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

    // printf(egl_display, egl_conf);

    EGLint queriedRenderBuffer;
    if (eglQueryContext(egl_display, egl_context, EGL_RENDER_BUFFER,
                        &queriedRenderBuffer)) {
        switch (queriedRenderBuffer) {
            case EGL_SINGLE_BUFFER:
                printf("Render Buffer: EGL_SINGLE_BUFFER");
                break;
            case EGL_BACK_BUFFER:
                printf("Render Buffer: EGL_BACK_BUFFER");
                break;
            case EGL_NONE:
                printf("Render Buffer: EGL_NONE");
                break;
            default:
                printf("Render Buffer: unknown value %d", queriedRenderBuffer);
                break;
        }
    } else {
        printf("Failed to query EGL_RENDER_BUFFER: %d", eglGetError());
    }

    if (!eglSwapInterval(egl_display, 1)) {
        printf("eglSwapInterval failed: %d", eglGetError());
    } else {
        printf("Set swap interval");
    }

    ///////  the openGL part
    //////////////////////////////////////////////////////////////////

    XWindowAttributes gwa;
    XGetWindowAttributes(x_display, win, &gwa);
    glViewport(0, 0, gwa.width, gwa.height);
    glClearColor(0.0, 1.0, 0.0, 1.0);

    // render loop
    for (;;) {
        glClear(GL_COLOR_BUFFER_BIT);

        eglSwapBuffers(egl_display, egl_surface);
    }

    return 0;
}
