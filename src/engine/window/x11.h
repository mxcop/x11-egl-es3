#pragma once

/**
 * @brief Initialize an X11 window.
 * 
 * @param display X11 display.
 * @param title Window title.
 * @return The initialized X11 window.
 */
Window init_x11_window(Display* display, int x, int y, int w, int h,
                       const char* title);

/**
 * @brief Cleanup an X11 window's resources.
 * 
 * @param d X11 display.
 * @param win X11 window.
 */
void x11_clean(Display* d, Window& win);
