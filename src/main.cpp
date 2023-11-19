#include <stdio.h>
#include <sys/time.h>
#include <thread>
#include <cassert>

#include <GLES2/gl2.h>
#include <X11/extensions/Xfixes.h>

#include "window/window.h"
#include "game/game.h"

double get_time() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<double>(tv.tv_sec + tv.tv_usec / 1000000.0);
}

/**
 * @brief Input thread.
 */
void input(GameWindow* win) {
    /* Register interest in window close events */
    Atom wm_delmsg = XInternAtom(win->x_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(win->x_display, win->x_win, &wm_delmsg, 1);

    /* "XNextEvent" is blocking */
    for (XEvent x_ev;; XNextEvent(win->x_display, &x_ev)) {
        /* Exit loop if window is closed */
        if (x_ev.type == ClientMessage && x_ev.xclient.data.l[0] == wm_delmsg) {
            win->open = false;
            return;
        }

        // XGrabPointer(win->x_display, win->x_win, True,
        //          ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
        //          GrabModeAsync, GrabModeAsync,
        //          win->x_win,
        //          None,
        //          CurrentTime);

        switch (x_ev.type) {
            case KeyPress:
                /* Exit on escape */
                if (XLookupKeysym(&x_ev.xkey, 0) == XK_Escape) {
                    win->open = false;
                    return;
                }
                if (XLookupKeysym(&x_ev.xkey, 0) == XK_space) {
                    printf("The space bar was pressed.\n");
                }
                break;
            case KeyRelease:
                if (XLookupKeysym(&x_ev.xkey, 0) == XK_space) {
                    printf("The space bar was released.\n");
                }
                break;
            case ButtonPress:
                if (x_ev.xbutton.button == Button1) {
                    printf("The left mouse button was pressed.\n");
                }
                break;
            case ButtonRelease:
                if (x_ev.xbutton.button == Button1) {
                    printf("The left mouse button was released.\n");
                }
                break;
            case MotionNotify:
                // TODO: change 400, 300 to screen width and height *.5
                //int dx = 400 - x_ev.xmotion.x, dy = 300 - x_ev.xmotion.y;
                //XWarpPointer(win->x_display, None, win->x_win, 0, 0, 0, 0, 400, 300);
                //XSetInputFocus(win->x_display, win->x_win, PointerRoot, CurrentTime);
                break;
            // case FocusIn:
            //     {
            //         int screen = DefaultScreen(win->x_display);
            //         Window root = RootWindow(win->x_display, screen);
            //         XFixesHideCursor(win->x_display, root);
            //         XSync(win->x_display, True);
            //         XFlush(win->x_display);
            //     }

            //     printf("Hide cursor.\n");
            //     break;
            // case FocusOut:
            //     {
            //         int screen = DefaultScreen(win->x_display);
            //         Window root = RootWindow(win->x_display, screen);
            //         XFixesShowCursor(win->x_display, root);
            //         XSync(win->x_display, False);
            //         XFlush(win->x_display);
            //     }

            //     printf("Show cursor.\n");
            //     break;
        }
    }
}

int main(int argc, char** argv) {
    /* Support for multi-threading */
    XInitThreads();

    /* Create the game window */
    GameWindow win = GameWindow();
    assert(win.init(800, 450, "Hello, world!"));
    win.vsync(false);

    /* Start input thread */
    std::thread input_th(input, &win);

    { /* Set viewport dimension */
        int w, h;
        win.size(w, h);
        glViewport(0, 0, w, h);
    }

    if (!init()) return -1; /* Game init */

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);
    glClearColor(0.0745f, 0.0784f, 0.0784f, 1.0f);

    double time = get_time();

    /* Program loop */
    while (win.open) {
        double dt = get_time() - time;
        // if (dt != 0.0) {
        //     printf("fps: %f\n", 1.0 / (get_time() - time));
        // }
        time = get_time();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        tick(dt); /* Game tick */

        win.swap_buffers();
    }

    input_th.join();
    return 0;
}
