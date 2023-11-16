#include "egl.h"

#include <stdio.h>

#include <X11/Xlib.h>
#include <EGL/egl.h>

int egl_init(Display* display, Window& window, EGLDisplay& out_egl_display,
             EGLSurface& out_egl_surface, EGLContext& out_egl_context) {
    /* Shorter names */
    EGLDisplay& egl_display = out_egl_display;
    EGLSurface& egl_surface = out_egl_surface;
    EGLContext& egl_context = out_egl_context;

    egl_display = eglGetDisplay((EGLNativeDisplayType)display);
    if (egl_display == EGL_NO_DISPLAY) {
        printf("Got no EGL display.\n");
        return 0;
    }

    EGLint egl_version_major, egl_version_minor;
    if (!eglInitialize(egl_display, &egl_version_major, &egl_version_minor)) {
        printf("Unable to initialize EGL.\n");
        return 0;
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
        return 0;
    }

    if (num_config != 1) {
        printf("Got %d configs...\n", num_config);
        return 0;
    }

    egl_surface = eglCreateWindowSurface(egl_display, egl_conf, window, NULL);
    if (egl_surface == EGL_NO_SURFACE) {
        printf("Unable to create EGL surface. (%d)\n", eglGetError());
        return 0;
    }

    EGLint ctxattr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    egl_context =
        eglCreateContext(egl_display, egl_conf, EGL_NO_CONTEXT, ctxattr);
    if (egl_context == EGL_NO_CONTEXT) {
        printf("Unable to create EGL context. (%d)\n", eglGetError());
        return 0;
    }

    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

    /* Print some info */
#if DEBUG
    printf("EGL > Client APIs: %s\n",
           eglQueryString(egl_display, EGL_CLIENT_APIS));
    printf("EGL >      Vendor: %s\n", eglQueryString(egl_display, EGL_VENDOR));
    printf("EGL >     Version: %s\n", eglQueryString(egl_display, EGL_VERSION));
    // printf("EGL - Extensions: \n%s\n", eglQueryString(egl_display,
    // EGL_EXTENSIONS));
#endif

    {
        EGLint queried_buffer;
        if (!eglQueryContext(egl_display, egl_context, EGL_RENDER_BUFFER,
                             &queried_buffer)) {
            printf("Warn: Failed to query EGL_RENDER_BUFFER: %d\n",
                   eglGetError());
        }
    }

    return 1;
}

int egl_vsync(EGLDisplay& egl_display, bool yes) {
    /* V-sync */
    if (!eglSwapInterval(egl_display, yes ? 1 : 0)) {
        printf("Warn: eglSwapInterval failed: %d\n", eglGetError());
        return 0;
    }
    return 1;
}

void egl_clean(EGLDisplay& d, EGLContext& ctx, EGLSurface& srf) {
    eglMakeCurrent(d, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(d, ctx);
    eglDestroySurface(d, srf);
    eglTerminate(d);
}
