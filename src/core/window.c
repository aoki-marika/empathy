#include "window.h"

// MARK: - Functions

void window_init(struct window_t *window,
                 unsigned int width,
                 unsigned int height,
                 const char *title)
{
    // create the window
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window->backing = glfwCreateWindow(width, height, title, NULL, NULL);

    // configure opengl
    window_set_current(window);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void window_deinit(struct window_t *window)
{
    glfwDestroyWindow(window->backing);
}

void window_set_current(struct window_t *window)
{
    glfwMakeContextCurrent(window->backing);
}

void window_set_background(struct window_t *window, const struct colour_t *colour)
{
    window_set_current(window);
    glClearColor(colour->r, colour->g, colour->b, colour->a);
}

bool window_is_closed(struct window_t *window)
{
    return glfwWindowShouldClose(window->backing) == GLFW_TRUE;
}

void window_begin_frame(struct window_t *window)
{
    // clear the new framebuffer
    window_set_current(window);
    glClear(GL_COLOR_BUFFER_BIT);
}

void window_end_frame(struct window_t *window)
{
    // display thew new framebuffer
    glfwSwapBuffers(window->backing);
}
