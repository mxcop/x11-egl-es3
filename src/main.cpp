#include <stdio.h>
#include <sys/time.h>
#include <thread>
#include <cassert>

#include <GLES2/gl2.h>

#include "engine/window.h"

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<double>(tv.tv_sec + tv.tv_usec / 1000000.0);
}

void input(GameWindow* win) {
    /* Register interest in window close events */
    Atom wm_delmsg = XInternAtom(win->x_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(win->x_display, win->x_win, &wm_delmsg, 1);

    /* "XNextEvent" is blocking */
    for (XEvent x_ev;;XNextEvent(win->x_display, &x_ev)) {
        /* Exit loop if window is closed */
        if (x_ev.type == ClientMessage && x_ev.xclient.data.l[0] == wm_delmsg) {
            win->open = false;
            return;
        }
    }
}

int main(int argc, char** argv) {
    /* Support for multi-threading */
    XInitThreads();

    /* Create the game window */
    GameWindow win = GameWindow();
    assert(win.init(800, 600, "Hello, world!"));
    win.vsync(false);

    /* Start input thread */
    std::thread input_th(input, &win);

    { /* Set viewport dimension */
        int w, h;
        win.size(w, h);
        glViewport(0, 0, w, h);
    }

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    double time = get_time();

    /* Program loop */
    while(win.open) {
        double dt = get_time() - time;
        // if (dt != 0.0) {
        //     printf("fps: %f\n", 1.0 / (get_time() - time));
        // }
        time = get_time();

        glClear(GL_COLOR_BUFFER_BIT);

        /* Do stuff here :) */

        win.swap_buffers();
    }

    input_th.join();
    return 0;
}
