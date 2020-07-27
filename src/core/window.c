#include "window.h"

// MARK: - Functions

void window_init(struct window_t *window,
                 unsigned int width,
                 unsigned int height,
                 const char *title,
                 bool is_resizable)
{
    // create the window
    glfwWindowHint(GLFW_RESIZABLE, (is_resizable) ? GLFW_TRUE : GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window->width = width;
    window->height = height;
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

void window_set_background(struct window_t *window, struct colour4_t colour)
{
    window_set_current(window);
    glClearColor(colour.r, colour.g, colour.b, colour.a);
}

bool window_is_closed(struct window_t *window)
{
    return glfwWindowShouldClose(window->backing) == GLFW_TRUE;
}

void window_begin_frame(struct window_t *window)
{
    window_set_current(window);
    glClear(GL_COLOR_BUFFER_BIT);
}

void window_begin_final_pass(struct window_t *window)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window->width, window->height);
    glClear(GL_COLOR_BUFFER_BIT);
}

void window_end_frame(struct window_t *window)
{
    glfwSwapBuffers(window->backing);
}
