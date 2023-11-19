#include "x11.h"

#include <libs.h>

Window init_x11_window(Display* display, int x, int y, int w, int h,
                       const char* title) {
    /* Get root window (usually whole screen) */
    Window root_win = DefaultRootWindow(display);

    XSetWindowAttributes swa;
    /* We're interested in these input events */
    swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                     ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                     FocusChangeMask;

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

void x11_clean(Display* d, Window& win) {
    XDestroyWindow(d, win);
    XCloseDisplay(d);
}
