#pragma once

/**
 * @brief Abstract window object.
 */
struct GameWindow {
    /* X11 */
    Window x_win = 0;
    Display* x_display = nullptr;

    /* EGL */
    EGLDisplay egl_display = nullptr;
    EGLSurface egl_surface = nullptr;
    EGLContext egl_context = nullptr;

    bool open = false;

    GameWindow() = default;

    /* Cannot be copied (could cause a dangling pointer/handle(s)) */
    GameWindow(const GameWindow&) = delete;
    GameWindow& operator=(const GameWindow&) = delete;

    /* Can be moved */
    GameWindow(GameWindow&&) = default;
    GameWindow& operator=(GameWindow&&) = default;

    ~GameWindow();

    int init(int w, int h, const char* title);
    int vsync(bool yes);

    /**
     * @brief Swap screen buffers. (called at the end of the program loop)
     */
    void swap_buffers() const;

    /**
     * @brief Get the size of the window in pixels.
     */
    void size(int& w, int& h) const;
};
