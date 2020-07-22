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
}

void window_deinit(struct window_t *window)
{
    glfwDestroyWindow(window->backing);
}

bool window_should_close(struct window_t *window)
{
    return glfwWindowShouldClose(window->backing) == GLFW_TRUE;
}

void window_make_current(struct window_t *window)
{
    glfwMakeContextCurrent(window->backing);
}

void window_swap_buffers(struct window_t *window)
{
    glfwSwapBuffers(window->backing);
}
