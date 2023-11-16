#include "window.h"

#include <stdio.h>

#include <libs.h>

#include "x11.h"
#include "egl.h"

int GameWindow::init(int w, int h, const char* title) {
    /* Get X11 display */
    x_display = XOpenDisplay(NULL);
    if (x_display == NULL) {
        printf("Cannot connect to X server.\n");
        return 0;
    }

    x_win = init_x11_window(x_display, 0, 0, w, h, "Hello, world!");

    /* Initialize the EGL context */
    if (!egl_init(x_display, x_win, egl_display, egl_surface, egl_context)) {
        printf("Error creating EGL context.\n");
        return 0;
    }

    open = true;
    return 1;
}

int GameWindow::vsync(bool yes) {
    if (open == false) return 0;
    return egl_vsync(egl_display, yes); 
}

void GameWindow::swap_buffers() const {
    if (open == false) return;
    eglSwapBuffers(egl_display, egl_surface);
}

void GameWindow::size(int& w, int& h) const {
    if (open == false) return;
    XWindowAttributes gwa;
    XGetWindowAttributes(x_display, x_win, &gwa);

    w = gwa.width;
    h = gwa.height;
}

GameWindow::~GameWindow() {
    if (x_display == nullptr) return;
    egl_clean(egl_display, egl_context, egl_surface);
    x11_clean(x_display, x_win);
}
