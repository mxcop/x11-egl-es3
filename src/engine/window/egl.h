#pragma once

/**
 * @brief (Attempt) to initialize the EGL context.
 */
int egl_init(Display* display, Window& window, EGLDisplay& out_egl_display,
                 EGLSurface& out_egl_surface, EGLContext& out_egl_context);

/**
 * @brief Set the vertical sync on an EGL context.
 */
int egl_vsync(EGLDisplay& egl_display, bool yes);

/**
 * @brief Properly cleanup EGL resources.
 */
void egl_clean(EGLDisplay& d, EGLContext& ctx, EGLSurface& srf);
